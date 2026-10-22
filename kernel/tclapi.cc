/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/rtlil.h"
#അന്തര്ഭാവ "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#അന്തര്ഭാവ <tcl.h>
#അന്തര്ഭാവ <tclTomMath.h>
#അന്തര്ഭാവ <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
തർക്കം yosys_tcl_repl_active = അസത്യം;

ശൂന്യം yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = സത്യം;
}

സ്ഥിതം Tcl_Obj *json_to_tcl(Tcl_Interp *interp, സ്ഥിരം json11::Json &json)
{
	എങ്കിൽ (json.is_null())
		തിരികെ Tcl_NewStringObj("null", 4);
	അല്ലെങ്കിൽ എങ്കിൽ (json.is_string()) {
		auto string = json.string_മൂല്യം();
		തിരികെ Tcl_NewStringObj(string.data(), string.size());
	} അല്ലെങ്കിൽ എങ്കിൽ (json.is_സംഖ്യ()) {
		ഇരട്ട മൂല്യം = json.സംഖ്യ_മൂല്യം();
		ഇരട്ട round_val = std::nearbyint(മൂല്യം);
		എങ്കിൽ (std::isfinite(round_val) && മൂല്യം == round_val && മൂല്യം >= LONG_MIN && മൂല്യം < -ഇരട്ട(LONG_MIN))
			തിരികെ Tcl_NewLongObj((long)round_val);
		അല്ലെങ്കിൽ
			തിരികെ Tcl_NewDoubleObj(മൂല്യം);
	} അല്ലെങ്കിൽ എങ്കിൽ (json.is_bool()) {
		തിരികെ Tcl_NewBooleanObj(json.bool_മൂല്യം());
	} അല്ലെങ്കിൽ എങ്കിൽ (json.is_array()) {
		auto list = json.array_ഇനങ്ങൾ();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		വേണ്ടി (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		തിരികെ result;
	} അല്ലെങ്കിൽ എങ്കിൽ (json.is_object()) {
		auto map = json.object_ഇനങ്ങൾ();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		വേണ്ടി (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		തിരികെ result;
	} അല്ലെങ്കിൽ {
		log_abort();
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tcl_yosys_cmd(ClientData, Tcl_Interp *interp, പൂർണ്ണസംഖ്യ argc, സ്ഥിരം അക്ഷരം *argv[])
{
	std::vector<std::string> args;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < argc; i++)
		args.തള്ളുക_back(argv[i]);

	എങ്കിൽ (args.size() >= 1 && args[0] == "-import") {
		വേണ്ടി (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			എങ്കിൽ (tcl_command_name == "proc")
				tcl_command_name = "procs";
			അല്ലെങ്കിൽ എങ്കിൽ (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			എങ്കിൽ (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} അല്ലെങ്കിൽ {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		തിരികെ TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	തർക്കം in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	ശ്രമിക്കുക {
		എങ്കിൽ (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} അല്ലെങ്കിൽ {
			Pass::call(yosys_get_design(), args);
		}
	} പിടിക്കുക (log_cmd_error_exception) {
		എങ്കിൽ (in_repl) {
			auto design = yosys_get_design();
			ആകുമ്പോൾ (design->selection_അട്ടി.size() > 1)
				design->എടുക്കുക_selection();
			log_reset_അട്ടി();
		}
		Tcl_SetResult(interp, (അക്ഷരം *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		തിരികെ TCL_ERROR;
	} പിടിക്കുക (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	എങ്കിൽ (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		എങ്കിൽ (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} അല്ലെങ്കിൽ
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} അല്ലെങ്കിൽ എങ്കിൽ ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	തിരികെ TCL_OK;
}

#define FLAG(name) \
	എങ്കിൽ (!strcmp(argv[i], "-" #name)) { \
		name##_flag = സത്യം; \
		തുടരുക; \
	} \

#define FLAG2(name) \
	എങ്കിൽ (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = സത്യം; \
		തുടരുക; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (അക്ഷരം *)(str), TCL_STATIC); \
		തിരികെ TCL_ERROR; \
	}

തർക്കം const_to_mp_int(സ്ഥിരം Const &a, mp_int *b, തർക്കം force_signed, തർക്കം force_unsigned)
{
	എങ്കിൽ (!a.is_fully_def())
		തിരികെ അസത്യം;

	എങ്കിൽ (mp_init(b))
		തിരികെ അസത്യം;

	തർക്കം negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = a.size() - 1; i >= 0; i--) {
		എങ്കിൽ (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			തിരികെ അസത്യം;
		}

		എങ്കിൽ ((a[i] == RTLIL::S1) ^ negative) {
			എങ്കിൽ (mp_add_d(b, 1, b)) {
				mp_clear(b);
				തിരികെ അസത്യം;
			}
		}
	}

	എങ്കിൽ (negative) {
		എങ്കിൽ (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			തിരികെ അസത്യം;
		}
	}

	തിരികെ സത്യം;
}

തർക്കം mp_int_to_const(mp_int *a, Const &b, തർക്കം is_signed)
{
	തർക്കം negative = (mp_cmp_d(a, 0) == MP_LT);
	എങ്കിൽ (negative && !is_signed)
		തിരികെ അസത്യം;

	എങ്കിൽ (negative) {
		എങ്കിൽ (mp_neg(a, a) != MP_OKAY)
			തിരികെ അസത്യം;
		എങ്കിൽ (mp_sub_d(a, 1, a) != MP_OKAY)
			തിരികെ അസത്യം;
	}

	std::vector<unsigned അക്ഷരം> buf;
	buf.resize(mp_unsigned_bin_size(a));
	എങ്കിൽ (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		തിരികെ അസത്യം;

	Const::Builder b_bits(mp_എണ്ണം_bits(a) + is_signed);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mp_എണ്ണം_bits(a);) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < 8 && i < mp_എണ്ണം_bits(a); j++, i++) {
			തർക്കം bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.തള്ളുക_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.എടുക്കുക_back();
	}

	എങ്കിൽ (is_signed) {
		b_bits.തള്ളുക_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	തിരികെ സത്യം;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tcl_get_attr(ClientData, Tcl_Interp *interp, പൂർണ്ണസംഖ്യ argc, സ്ഥിരം അക്ഷരം *argv[])
{
	പൂർണ്ണസംഖ്യ i;
	തർക്കം mod_flag = അസത്യം, string_flag = അസത്യം, bool_flag = അസത്യം;
	തർക്കം int_flag = അസത്യം, sint_flag = അസത്യം, uint_flag = അസത്യം;
	വേണ്ടി (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(പൂർണ്ണസംഖ്യ)
		FLAG(sint)
		FLAG(uint)
		FLAG(തർക്കം)
		നിർത്തുക;
	}

	എങ്കിൽ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	എങ്കിൽ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	എങ്കിൽ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	എങ്കിൽ (mod_flag) {
		obj = mod;
	} അല്ലെങ്കിൽ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		എങ്കിൽ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		എങ്കിൽ (!obj)
			obj = mod->cell(obj_twine);
		എങ്കിൽ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	എങ്കിൽ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	എങ്കിൽ (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} അല്ലെങ്കിൽ എങ്കിൽ (int_flag || uint_flag || sint_flag) {
		എങ്കിൽ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &മൂല്യം = obj->attributes.at(attr_twine);

		mp_int മൂല്യം_mp;
		എങ്കിൽ (!const_to_mp_int(മൂല്യം, &മൂല്യം_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&മൂല്യം_mp));
	} അല്ലെങ്കിൽ എങ്കിൽ (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	തിരികെ TCL_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tcl_has_attr(ClientData, Tcl_Interp *interp, പൂർണ്ണസംഖ്യ argc, സ്ഥിരം അക്ഷരം *argv[])
{
	പൂർണ്ണസംഖ്യ i;
	തർക്കം mod_flag = അസത്യം;
	വേണ്ടി (i = 1; i < argc; i++) {
		FLAG(mod)
		നിർത്തുക;
	}

	എങ്കിൽ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	എങ്കിൽ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	എങ്കിൽ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	എങ്കിൽ (mod_flag) {
		obj = mod;
	} അല്ലെങ്കിൽ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		എങ്കിൽ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		എങ്കിൽ (!obj)
			obj = mod->cell(obj_twine);
		എങ്കിൽ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	എങ്കിൽ (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	തിരികെ TCL_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tcl_set_attr(ClientData, Tcl_Interp *interp, പൂർണ്ണസംഖ്യ objc, Tcl_Obj *സ്ഥിരം objv[])
{
	പൂർണ്ണസംഖ്യ i;
	തർക്കം mod_flag = അസത്യം, string_flag = അസത്യം, bool_flag = അസത്യം;
	തർക്കം true_flag = അസത്യം, false_flag = അസത്യം, sint_flag = അസത്യം, uint_flag = അസത്യം;
	വേണ്ടി (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(സത്യം)
		FLAG2(അസത്യം)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(തർക്കം)
		നിർത്തുക;
	}

	എങ്കിൽ ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	എങ്കിൽ (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	എങ്കിൽ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	എങ്കിൽ (mod_flag) {
		obj = mod;
	} അല്ലെങ്കിൽ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		എങ്കിൽ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		എങ്കിൽ (!obj)
			obj = mod->cell(obj_twine);
		എങ്കിൽ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	എങ്കിൽ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	എങ്കിൽ (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} അല്ലെങ്കിൽ എങ്കിൽ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int മൂല്യം_mp;

		എങ്കിൽ (Tcl_TakeBignumFromObj(interp, objv[i++], &മൂല്യം_mp))
			ERROR("non-integral value")

		എങ്കിൽ (!mp_int_to_const(&മൂല്യം_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		എങ്കിൽ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			എങ്കിൽ (const_.size() < 32)
				const_.exts(32);
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} അല്ലെങ്കിൽ എങ്കിൽ (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} അല്ലെങ്കിൽ എങ്കിൽ (true_flag) {
		obj->set_bool_attribute(attr_twine, സത്യം);
	} അല്ലെങ്കിൽ എങ്കിൽ (false_flag) {
		obj->set_bool_attribute(attr_twine, അസത്യം);
	} അല്ലെങ്കിൽ {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	തിരികെ TCL_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tcl_get_param(ClientData, Tcl_Interp *interp, പൂർണ്ണസംഖ്യ argc, സ്ഥിരം അക്ഷരം *argv[])
{
	പൂർണ്ണസംഖ്യ i;
	തർക്കം string_flag = അസത്യം;
	തർക്കം int_flag = അസത്യം, sint_flag = അസത്യം, uint_flag = അസത്യം;
	വേണ്ടി (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(പൂർണ്ണസംഖ്യ)
		FLAG(sint)
		FLAG(uint)
		നിർത്തുക;
	}

	എങ്കിൽ ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	എങ്കിൽ (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	എങ്കിൽ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	എങ്കിൽ (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	സ്ഥിരം RTLIL::Const &മൂല്യം = cell->getParam(param_twine);

	എങ്കിൽ (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(മൂല്യം.decode_string().c_str(), -1));
	} അല്ലെങ്കിൽ എങ്കിൽ (int_flag || uint_flag || sint_flag) {
		mp_int മൂല്യം_mp;
		എങ്കിൽ (!const_to_mp_int(മൂല്യം, &മൂല്യം_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&മൂല്യം_mp));
	} അല്ലെങ്കിൽ {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(മൂല്യം.as_string().c_str(), -1));
	}
	തിരികെ TCL_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tcl_set_param(ClientData, Tcl_Interp *interp, പൂർണ്ണസംഖ്യ objc, Tcl_Obj *സ്ഥിരം objv[])
{
	പൂർണ്ണസംഖ്യ i;
	തർക്കം string_flag = അസത്യം, sint_flag = അസത്യം, uint_flag = അസത്യം;
	വേണ്ടി (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		നിർത്തുക;
	}

	എങ്കിൽ ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	എങ്കിൽ (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	എങ്കിൽ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	എങ്കിൽ (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} അല്ലെങ്കിൽ എങ്കിൽ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int മൂല്യം_mp;

		എങ്കിൽ (Tcl_TakeBignumFromObj(interp, objv[i++], &മൂല്യം_mp))
			ERROR("non-integral value")

		എങ്കിൽ (!mp_int_to_const(&മൂല്യം_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		എങ്കിൽ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			എങ്കിൽ (const_.size() < 32)
				const_.exts(32);
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} അല്ലെങ്കിൽ {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	തിരികെ TCL_OK;
}

പൂർണ്ണസംഖ്യ yosys_tcl_interp_init(Tcl_Interp *interp)
{
	എങ്കിൽ (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_സന്ദേശം(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ശൂന്യ, ശൂന്യ);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ശൂന്യ, ശൂന്യ);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ശൂന്യ, ശൂന്യ);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, ശൂന്യ, ശൂന്യ);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ശൂന്യ, ശൂന്യ);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, ശൂന്യ, ശൂന്യ);

	// TODO:
	//
	// port_list
	// wire_list
	// cell_list
	// wire_width
	//
	// add_wire
	// add_cell
	// rename_wire
	// rename_cell
	// remove
	//
	// SigSpec land
	//
	// get_conn
	// set_conn
	// unpack
	// pack

	// Note (dev jf 24-12-02): Make log_id escape everything that’s not a valid
	// verilog identifier before adding any tcl API that returns IdString values
	// to avoid -option injection

	തിരികെ TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
