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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/rtlil.h"
#அந்தர்பாவ "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#அந்தர்பாவ <tcl.h>
#அந்தர்பாவ <tclTomMath.h>
#அந்தர்பாவ <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
தருக்க yosys_tcl_repl_active = பொய்;

வெறுமை yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = உண்மை;
}

நிலையான Tcl_Obj *json_to_tcl(Tcl_Interp *interp, மாறிலி json11::Json &json)
{
	எனில் (json.is_null())
		திரும்பு Tcl_NewStringObj("null", 4);
	இல்லையெனில் எனில் (json.is_string()) {
		auto string = json.string_மதிப்பு();
		திரும்பு Tcl_NewStringObj(string.data(), string.size());
	} இல்லையெனில் எனில் (json.is_எண்()) {
		இரட்டை மதிப்பு = json.எண்_மதிப்பு();
		இரட்டை round_val = std::nearbyint(மதிப்பு);
		எனில் (std::isfinite(round_val) && மதிப்பு == round_val && மதிப்பு >= LONG_MIN && மதிப்பு < -இரட்டை(LONG_MIN))
			திரும்பு Tcl_NewLongObj((long)round_val);
		இல்லையெனில்
			திரும்பு Tcl_NewDoubleObj(மதிப்பு);
	} இல்லையெனில் எனில் (json.is_bool()) {
		திரும்பு Tcl_NewBooleanObj(json.bool_மதிப்பு());
	} இல்லையெனில் எனில் (json.is_array()) {
		auto list = json.array_பொருட்கள்();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		க்கு (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		திரும்பு result;
	} இல்லையெனில் எனில் (json.is_object()) {
		auto map = json.object_பொருட்கள்();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		க்கு (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		திரும்பு result;
	} இல்லையெனில் {
		log_abort();
	}
}

நிலையான முழு tcl_yosys_cmd(ClientData, Tcl_Interp *interp, முழு argc, மாறிலி எழுத்து *argv[])
{
	std::vector<std::string> args;
	க்கு (முழு i = 1; i < argc; i++)
		args.தள்ளு_back(argv[i]);

	எனில் (args.size() >= 1 && args[0] == "-import") {
		க்கு (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			எனில் (tcl_command_name == "proc")
				tcl_command_name = "procs";
			இல்லையெனில் எனில் (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			எனில் (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} இல்லையெனில் {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		திரும்பு TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	தருக்க in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	முயல் {
		எனில் (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} இல்லையெனில் {
			Pass::call(yosys_get_design(), args);
		}
	} பிடி (log_cmd_error_exception) {
		எனில் (in_repl) {
			auto design = yosys_get_design();
			போது (design->selection_அடுக்கு.size() > 1)
				design->எடு_selection();
			log_reset_அடுக்கு();
		}
		Tcl_SetResult(interp, (எழுத்து *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		திரும்பு TCL_ERROR;
	} பிடி (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	எனில் (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		எனில் (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} இல்லையெனில்
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} இல்லையெனில் எனில் ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	திரும்பு TCL_OK;
}

#define FLAG(name) \
	எனில் (!strcmp(argv[i], "-" #name)) { \
		name##_flag = உண்மை; \
		தொடர்; \
	} \

#define FLAG2(name) \
	எனில் (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = உண்மை; \
		தொடர்; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (எழுத்து *)(str), TCL_STATIC); \
		திரும்பு TCL_ERROR; \
	}

தருக்க const_to_mp_int(மாறிலி Const &a, mp_int *b, தருக்க force_signed, தருக்க force_unsigned)
{
	எனில் (!a.is_fully_def())
		திரும்பு பொய்;

	எனில் (mp_init(b))
		திரும்பு பொய்;

	தருக்க negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	க்கு (முழு i = a.size() - 1; i >= 0; i--) {
		எனில் (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			திரும்பு பொய்;
		}

		எனில் ((a[i] == RTLIL::S1) ^ negative) {
			எனில் (mp_add_d(b, 1, b)) {
				mp_clear(b);
				திரும்பு பொய்;
			}
		}
	}

	எனில் (negative) {
		எனில் (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			திரும்பு பொய்;
		}
	}

	திரும்பு உண்மை;
}

தருக்க mp_int_to_const(mp_int *a, Const &b, தருக்க is_signed)
{
	தருக்க negative = (mp_cmp_d(a, 0) == MP_LT);
	எனில் (negative && !is_signed)
		திரும்பு பொய்;

	எனில் (negative) {
		எனில் (mp_neg(a, a) != MP_OKAY)
			திரும்பு பொய்;
		எனில் (mp_sub_d(a, 1, a) != MP_OKAY)
			திரும்பு பொய்;
	}

	std::vector<unsigned எழுத்து> buf;
	buf.resize(mp_unsigned_bin_size(a));
	எனில் (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		திரும்பு பொய்;

	Const::Builder b_bits(mp_மொத்தம்_bits(a) + is_signed);
	க்கு (முழு i = 0; i < mp_மொத்தம்_bits(a);) {
		க்கு (முழு j = 0; j < 8 && i < mp_மொத்தம்_bits(a); j++, i++) {
			தருக்க bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.தள்ளு_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.எடு_back();
	}

	எனில் (is_signed) {
		b_bits.தள்ளு_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	திரும்பு உண்மை;
}

நிலையான முழு tcl_get_attr(ClientData, Tcl_Interp *interp, முழு argc, மாறிலி எழுத்து *argv[])
{
	முழு i;
	தருக்க mod_flag = பொய், string_flag = பொய், bool_flag = பொய்;
	தருக்க int_flag = பொய், sint_flag = பொய், uint_flag = பொய்;
	க்கு (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(முழு)
		FLAG(sint)
		FLAG(uint)
		FLAG(தருக்க)
		நிறுத்து;
	}

	எனில் ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	எனில் (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	எனில் (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	எனில் (mod_flag) {
		obj = mod;
	} இல்லையெனில் {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		எனில் (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		எனில் (!obj)
			obj = mod->cell(obj_twine);
		எனில் (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	எனில் (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	எனில் (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} இல்லையெனில் எனில் (int_flag || uint_flag || sint_flag) {
		எனில் (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &மதிப்பு = obj->attributes.at(attr_twine);

		mp_int மதிப்பு_mp;
		எனில் (!const_to_mp_int(மதிப்பு, &மதிப்பு_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&மதிப்பு_mp));
	} இல்லையெனில் எனில் (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} இல்லையெனில் {
		எனில் (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	திரும்பு TCL_OK;
}

நிலையான முழு tcl_has_attr(ClientData, Tcl_Interp *interp, முழு argc, மாறிலி எழுத்து *argv[])
{
	முழு i;
	தருக்க mod_flag = பொய்;
	க்கு (i = 1; i < argc; i++) {
		FLAG(mod)
		நிறுத்து;
	}

	எனில் ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	எனில் (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	எனில் (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	எனில் (mod_flag) {
		obj = mod;
	} இல்லையெனில் {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		எனில் (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		எனில் (!obj)
			obj = mod->cell(obj_twine);
		எனில் (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	எனில் (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	திரும்பு TCL_OK;
}

நிலையான முழு tcl_set_attr(ClientData, Tcl_Interp *interp, முழு objc, Tcl_Obj *மாறிலி objv[])
{
	முழு i;
	தருக்க mod_flag = பொய், string_flag = பொய், bool_flag = பொய்;
	தருக்க true_flag = பொய், false_flag = பொய், sint_flag = பொய், uint_flag = பொய்;
	க்கு (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(உண்மை)
		FLAG2(பொய்)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(தருக்க)
		நிறுத்து;
	}

	எனில் ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	எனில் (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	எனில் (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	எனில் (mod_flag) {
		obj = mod;
	} இல்லையெனில் {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		எனில் (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		எனில் (!obj)
			obj = mod->cell(obj_twine);
		எனில் (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	எனில் (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	எனில் (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} இல்லையெனில் எனில் (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int மதிப்பு_mp;

		எனில் (Tcl_TakeBignumFromObj(interp, objv[i++], &மதிப்பு_mp))
			ERROR("non-integral value")

		எனில் (!mp_int_to_const(&மதிப்பு_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		எனில் (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			எனில் (const_.size() < 32)
				const_.exts(32);
		} இல்லையெனில் {
			எனில் (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} இல்லையெனில் எனில் (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} இல்லையெனில் எனில் (true_flag) {
		obj->set_bool_attribute(attr_twine, உண்மை);
	} இல்லையெனில் எனில் (false_flag) {
		obj->set_bool_attribute(attr_twine, பொய்);
	} இல்லையெனில் {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	திரும்பு TCL_OK;
}

நிலையான முழு tcl_get_param(ClientData, Tcl_Interp *interp, முழு argc, மாறிலி எழுத்து *argv[])
{
	முழு i;
	தருக்க string_flag = பொய்;
	தருக்க int_flag = பொய், sint_flag = பொய், uint_flag = பொய்;
	க்கு (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(முழு)
		FLAG(sint)
		FLAG(uint)
		நிறுத்து;
	}

	எனில் ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	எனில் (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	எனில் (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	எனில் (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	மாறிலி RTLIL::Const &மதிப்பு = cell->getParam(param_twine);

	எனில் (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(மதிப்பு.decode_string().c_str(), -1));
	} இல்லையெனில் எனில் (int_flag || uint_flag || sint_flag) {
		mp_int மதிப்பு_mp;
		எனில் (!const_to_mp_int(மதிப்பு, &மதிப்பு_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&மதிப்பு_mp));
	} இல்லையெனில் {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(மதிப்பு.as_string().c_str(), -1));
	}
	திரும்பு TCL_OK;
}

நிலையான முழு tcl_set_param(ClientData, Tcl_Interp *interp, முழு objc, Tcl_Obj *மாறிலி objv[])
{
	முழு i;
	தருக்க string_flag = பொய், sint_flag = பொய், uint_flag = பொய்;
	க்கு (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		நிறுத்து;
	}

	எனில் ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	எனில் (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	எனில் (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	எனில் (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} இல்லையெனில் எனில் (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int மதிப்பு_mp;

		எனில் (Tcl_TakeBignumFromObj(interp, objv[i++], &மதிப்பு_mp))
			ERROR("non-integral value")

		எனில் (!mp_int_to_const(&மதிப்பு_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		எனில் (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			எனில் (const_.size() < 32)
				const_.exts(32);
		} இல்லையெனில் {
			எனில் (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} இல்லையெனில் {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	திரும்பு TCL_OK;
}

முழு yosys_tcl_interp_init(Tcl_Interp *interp)
{
	எனில் (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_செய்தி(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, வெற்று, வெற்று);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, வெற்று, வெற்று);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, வெற்று, வெற்று);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, வெற்று, வெற்று);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, வெற்று, வெற்று);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, வெற்று, வெற்று);

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

	திரும்பு TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
