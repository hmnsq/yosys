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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/rtlil.h"
#અન્તર્ભાવ "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#અન્તર્ભાવ <tcl.h>
#અન્તર્ભાવ <tclTomMath.h>
#અન્તર્ભાવ <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
તાર્કિક yosys_tcl_repl_active = ખોટું;

શૂન્ય yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = સાચું;
}

સ્થિર Tcl_Obj *json_to_tcl(Tcl_Interp *interp, અચલ json11::Json &json)
{
	જો (json.is_null())
		પાછા Tcl_NewStringObj("null", 4);
	નહીંતર જો (json.is_string()) {
		auto string = json.string_કિંમત();
		પાછા Tcl_NewStringObj(string.data(), string.size());
	} નહીંતર જો (json.is_સંખ્યા()) {
		દ્વિગુણ કિંમત = json.સંખ્યા_કિંમત();
		દ્વિગુણ round_val = std::nearbyint(કિંમત);
		જો (std::isfinite(round_val) && કિંમત == round_val && કિંમત >= LONG_MIN && કિંમત < -દ્વિગુણ(LONG_MIN))
			પાછા Tcl_NewLongObj((long)round_val);
		નહીંતર
			પાછા Tcl_NewDoubleObj(કિંમત);
	} નહીંતર જો (json.is_bool()) {
		પાછા Tcl_NewBooleanObj(json.bool_કિંમત());
	} નહીંતર જો (json.is_array()) {
		auto list = json.array_વસ્તુઓ();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		માટે (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		પાછા result;
	} નહીંતર જો (json.is_object()) {
		auto map = json.object_વસ્તુઓ();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		માટે (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		પાછા result;
	} નહીંતર {
		log_abort();
	}
}

સ્થિર પૂર્ણાંક tcl_yosys_cmd(ClientData, Tcl_Interp *interp, પૂર્ણાંક argc, અચલ અક્ષર *argv[])
{
	std::vector<std::string> args;
	માટે (પૂર્ણાંક i = 1; i < argc; i++)
		args.ધકેલો_back(argv[i]);

	જો (args.size() >= 1 && args[0] == "-import") {
		માટે (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			જો (tcl_command_name == "proc")
				tcl_command_name = "procs";
			નહીંતર જો (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			જો (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} નહીંતર {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		પાછા TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	તાર્કિક in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	પ્રયાસ {
		જો (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} નહીંતર {
			Pass::call(yosys_get_design(), args);
		}
	} પકડો (log_cmd_error_exception) {
		જો (in_repl) {
			auto design = yosys_get_design();
			જ્યારે (design->selection_ઢગલો.size() > 1)
				design->કાઢો_selection();
			log_reset_ઢગલો();
		}
		Tcl_SetResult(interp, (અક્ષર *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		પાછા TCL_ERROR;
	} પકડો (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	જો (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		જો (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} નહીંતર
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} નહીંતર જો ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	પાછા TCL_OK;
}

#define FLAG(name) \
	જો (!strcmp(argv[i], "-" #name)) { \
		name##_flag = સાચું; \
		ચાલુ; \
	} \

#define FLAG2(name) \
	જો (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = સાચું; \
		ચાલુ; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (અક્ષર *)(str), TCL_STATIC); \
		પાછા TCL_ERROR; \
	}

તાર્કિક const_to_mp_int(અચલ Const &a, mp_int *b, તાર્કિક force_signed, તાર્કિક force_unsigned)
{
	જો (!a.is_fully_def())
		પાછા ખોટું;

	જો (mp_init(b))
		પાછા ખોટું;

	તાર્કિક negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	માટે (પૂર્ણાંક i = a.size() - 1; i >= 0; i--) {
		જો (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			પાછા ખોટું;
		}

		જો ((a[i] == RTLIL::S1) ^ negative) {
			જો (mp_add_d(b, 1, b)) {
				mp_clear(b);
				પાછા ખોટું;
			}
		}
	}

	જો (negative) {
		જો (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			પાછા ખોટું;
		}
	}

	પાછા સાચું;
}

તાર્કિક mp_int_to_const(mp_int *a, Const &b, તાર્કિક is_signed)
{
	તાર્કિક negative = (mp_cmp_d(a, 0) == MP_LT);
	જો (negative && !is_signed)
		પાછા ખોટું;

	જો (negative) {
		જો (mp_neg(a, a) != MP_OKAY)
			પાછા ખોટું;
		જો (mp_sub_d(a, 1, a) != MP_OKAY)
			પાછા ખોટું;
	}

	std::vector<unsigned અક્ષર> buf;
	buf.resize(mp_unsigned_bin_size(a));
	જો (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		પાછા ખોટું;

	Const::Builder b_bits(mp_ગણતરી_bits(a) + is_signed);
	માટે (પૂર્ણાંક i = 0; i < mp_ગણતરી_bits(a);) {
		માટે (પૂર્ણાંક j = 0; j < 8 && i < mp_ગણતરી_bits(a); j++, i++) {
			તાર્કિક bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.ધકેલો_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.કાઢો_back();
	}

	જો (is_signed) {
		b_bits.ધકેલો_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	પાછા સાચું;
}

સ્થિર પૂર્ણાંક tcl_get_attr(ClientData, Tcl_Interp *interp, પૂર્ણાંક argc, અચલ અક્ષર *argv[])
{
	પૂર્ણાંક i;
	તાર્કિક mod_flag = ખોટું, string_flag = ખોટું, bool_flag = ખોટું;
	તાર્કિક int_flag = ખોટું, sint_flag = ખોટું, uint_flag = ખોટું;
	માટે (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(પૂર્ણાંક)
		FLAG(sint)
		FLAG(uint)
		FLAG(તાર્કિક)
		તોડો;
	}

	જો ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	જો (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	જો (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	જો (mod_flag) {
		obj = mod;
	} નહીંતર {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		જો (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		જો (!obj)
			obj = mod->cell(obj_twine);
		જો (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	જો (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	જો (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} નહીંતર જો (int_flag || uint_flag || sint_flag) {
		જો (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &કિંમત = obj->attributes.at(attr_twine);

		mp_int કિંમત_mp;
		જો (!const_to_mp_int(કિંમત, &કિંમત_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&કિંમત_mp));
	} નહીંતર જો (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} નહીંતર {
		જો (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	પાછા TCL_OK;
}

સ્થિર પૂર્ણાંક tcl_has_attr(ClientData, Tcl_Interp *interp, પૂર્ણાંક argc, અચલ અક્ષર *argv[])
{
	પૂર્ણાંક i;
	તાર્કિક mod_flag = ખોટું;
	માટે (i = 1; i < argc; i++) {
		FLAG(mod)
		તોડો;
	}

	જો ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	જો (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	જો (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	જો (mod_flag) {
		obj = mod;
	} નહીંતર {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		જો (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		જો (!obj)
			obj = mod->cell(obj_twine);
		જો (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	જો (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	પાછા TCL_OK;
}

સ્થિર પૂર્ણાંક tcl_set_attr(ClientData, Tcl_Interp *interp, પૂર્ણાંક objc, Tcl_Obj *અચલ objv[])
{
	પૂર્ણાંક i;
	તાર્કિક mod_flag = ખોટું, string_flag = ખોટું, bool_flag = ખોટું;
	તાર્કિક true_flag = ખોટું, false_flag = ખોટું, sint_flag = ખોટું, uint_flag = ખોટું;
	માટે (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(સાચું)
		FLAG2(ખોટું)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(તાર્કિક)
		તોડો;
	}

	જો ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	જો (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	જો (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	જો (mod_flag) {
		obj = mod;
	} નહીંતર {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		જો (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		જો (!obj)
			obj = mod->cell(obj_twine);
		જો (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	જો (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	જો (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} નહીંતર જો (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int કિંમત_mp;

		જો (Tcl_TakeBignumFromObj(interp, objv[i++], &કિંમત_mp))
			ERROR("non-integral value")

		જો (!mp_int_to_const(&કિંમત_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		જો (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			જો (const_.size() < 32)
				const_.exts(32);
		} નહીંતર {
			જો (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} નહીંતર જો (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} નહીંતર જો (true_flag) {
		obj->set_bool_attribute(attr_twine, સાચું);
	} નહીંતર જો (false_flag) {
		obj->set_bool_attribute(attr_twine, ખોટું);
	} નહીંતર {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	પાછા TCL_OK;
}

સ્થિર પૂર્ણાંક tcl_get_param(ClientData, Tcl_Interp *interp, પૂર્ણાંક argc, અચલ અક્ષર *argv[])
{
	પૂર્ણાંક i;
	તાર્કિક string_flag = ખોટું;
	તાર્કિક int_flag = ખોટું, sint_flag = ખોટું, uint_flag = ખોટું;
	માટે (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(પૂર્ણાંક)
		FLAG(sint)
		FLAG(uint)
		તોડો;
	}

	જો ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	જો (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	જો (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	જો (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	અચલ RTLIL::Const &કિંમત = cell->getParam(param_twine);

	જો (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(કિંમત.decode_string().c_str(), -1));
	} નહીંતર જો (int_flag || uint_flag || sint_flag) {
		mp_int કિંમત_mp;
		જો (!const_to_mp_int(કિંમત, &કિંમત_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&કિંમત_mp));
	} નહીંતર {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(કિંમત.as_string().c_str(), -1));
	}
	પાછા TCL_OK;
}

સ્થિર પૂર્ણાંક tcl_set_param(ClientData, Tcl_Interp *interp, પૂર્ણાંક objc, Tcl_Obj *અચલ objv[])
{
	પૂર્ણાંક i;
	તાર્કિક string_flag = ખોટું, sint_flag = ખોટું, uint_flag = ખોટું;
	માટે (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		તોડો;
	}

	જો ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	જો (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	જો (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	જો (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} નહીંતર જો (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int કિંમત_mp;

		જો (Tcl_TakeBignumFromObj(interp, objv[i++], &કિંમત_mp))
			ERROR("non-integral value")

		જો (!mp_int_to_const(&કિંમત_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		જો (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			જો (const_.size() < 32)
				const_.exts(32);
		} નહીંતર {
			જો (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} નહીંતર {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	પાછા TCL_OK;
}

પૂર્ણાંક yosys_tcl_interp_init(Tcl_Interp *interp)
{
	જો (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_સંદેશ(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ખાલી, ખાલી);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ખાલી, ખાલી);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ખાલી, ખાલી);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, ખાલી, ખાલી);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ખાલી, ખાલી);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, ખાલી, ખાલી);

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

	પાછા TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
