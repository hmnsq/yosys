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

#include "kernel/yosys.h"
#include "kernel/rtlil.h"
#include "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#include <tcl.h>
#include <tclTomMath.h>
#include <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
bool yosys_tcl_repl_active = false;

void yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = true;
}

static Tcl_Obj *json_to_tcl(Tcl_Interp *interp, const json11::Json &json)
{
	གལ་སྲིད (json.is_null())
		ལོག Tcl_NewStringObj("null", 4);
	དེ་མིན གལ་སྲིད (json.is_string()) {
		auto string = json.string_value();
		ལོག Tcl_NewStringObj(string.data(), string.size());
	} དེ་མིན གལ་སྲིད (json.is_number()) {
		dviguu_na value = json.number_value();
		dviguu_na round_val = std::nearbyint(value);
		གལ་སྲིད (std::isfinite(round_val) && value == round_val && value >= LONG_MIN && value < -dviguu_na(LONG_MIN))
			ལོག Tcl_NewLongObj((long)round_val);
		དེ་མིན
			ལོག Tcl_NewDoubleObj(value);
	} དེ་མིན གལ་སྲིད (json.is_bool()) {
		ལོག Tcl_NewBooleanObj(json.bool_value());
	} དེ་མིན གལ་སྲིད (json.is_array()) {
		auto list = json.array_items();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		དོན་ལུ (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		ལོག result;
	} དེ་མིན གལ་སྲིད (json.is_object()) {
		auto map = json.object_items();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		དོན་ལུ (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		ལོག result;
	} དེ་མིན {
		log_abort();
	}
}

static ཧྲིལ tcl_yosys_cmd(ClientData, Tcl_Interp *interp, ཧྲིལ argc, const var_na *argv[])
{
	std::vector<std::string> args;
	དོན་ལུ (ཧྲིལ i = 1; i < argc; i++)
		args.push_back(argv[i]);

	གལ་སྲིད (args.size() >= 1 && args[0] == "-import") {
		དོན་ལུ (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			གལ་སྲིད (tcl_command_name == "proc")
				tcl_command_name = "procs";
			དེ་མིན གལ་སྲིད (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			གལ་སྲིད (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} དེ་མིན {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ལོག TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	bool in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	try {
		གལ་སྲིད (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} དེ་མིན {
			Pass::call(yosys_get_design(), args);
		}
	} gr_haa_na (log_cmd_error_exception) {
		གལ་སྲིད (in_repl) {
			auto design = yosys_get_design();
			སྐབས (design->selection_stack.size() > 1)
				design->pop_selection();
			log_reset_stack();
		}
		Tcl_SetResult(interp, (var_na *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		ལོག TCL_ERROR;
	} gr_haa_na (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	གལ་སྲིད (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		གལ་སྲིད (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} དེ་མིན
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} དེ་མིན གལ་སྲིད ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	ལོག TCL_OK;
}

#define FLAG(name) \
	གལ་སྲིད (!strcmp(argv[i], "-" #name)) { \
		name##_flag = true; \
		continue; \
	} \

#define FLAG2(name) \
	གལ་སྲིད (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = true; \
		continue; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (var_na *)(str), TCL_STATIC); \
		ལོག TCL_ERROR; \
	}

bool const_to_mp_int(const Const &a, mp_int *b, bool force_signed, bool force_unsigned)
{
	གལ་སྲིད (!a.is_fully_def())
		ལོག false;

	གལ་སྲིད (mp_init(b))
		ལོག false;

	bool negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	དོན་ལུ (ཧྲིལ i = a.size() - 1; i >= 0; i--) {
		གལ་སྲིད (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ལོག false;
		}

		གལ་སྲིད ((a[i] == RTLIL::S1) ^ negative) {
			གལ་སྲིད (mp_add_d(b, 1, b)) {
				mp_clear(b);
				ལོག false;
			}
		}
	}

	གལ་སྲིད (negative) {
		གལ་སྲིད (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ལོག false;
		}
	}

	ལོག true;
}

bool mp_int_to_const(mp_int *a, Const &b, bool is_signed)
{
	bool negative = (mp_cmp_d(a, 0) == MP_LT);
	གལ་སྲིད (negative && !is_signed)
		ལོག false;

	གལ་སྲིད (negative) {
		གལ་སྲིད (mp_neg(a, a) != MP_OKAY)
			ལོག false;
		གལ་སྲིད (mp_sub_d(a, 1, a) != MP_OKAY)
			ལོག false;
	}

	std::vector<unsigned var_na> buf;
	buf.resize(mp_unsigned_bin_size(a));
	གལ་སྲིད (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		ལོག false;

	Const::Builder b_bits(mp_count_bits(a) + is_signed);
	དོན་ལུ (ཧྲིལ i = 0; i < mp_count_bits(a);) {
		དོན་ལུ (ཧྲིལ j = 0; j < 8 && i < mp_count_bits(a); j++, i++) {
			bool bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.push_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.pop_back();
	}

	གལ་སྲིད (is_signed) {
		b_bits.push_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	ལོག true;
}

static ཧྲིལ tcl_get_attr(ClientData, Tcl_Interp *interp, ཧྲིལ argc, const var_na *argv[])
{
	ཧྲིལ i;
	bool mod_flag = false, string_flag = false, bool_flag = false;
	bool int_flag = false, sint_flag = false, uint_flag = false;
	དོན་ལུ (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(ཧྲིལ)
		FLAG(sint)
		FLAG(uint)
		FLAG(bool)
		bha_nga;
	}

	གལ་སྲིད ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	གལ་སྲིད (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	གལ་སྲིད (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	གལ་སྲིད (mod_flag) {
		obj = mod;
	} དེ་མིན {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		གལ་སྲིད (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		གལ་སྲིད (!obj)
			obj = mod->cell(obj_twine);
		གལ་སྲིད (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	གལ་སྲིད (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	གལ་སྲིད (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} དེ་མིན གལ་སྲིད (int_flag || uint_flag || sint_flag) {
		གལ་སྲིད (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &value = obj->attributes.at(attr_twine);

		mp_int value_mp;
		གལ་སྲིད (!const_to_mp_int(value, &value_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&value_mp));
	} དེ་མིན གལ་སྲིད (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} དེ་མིན {
		གལ་སྲིད (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	ལོག TCL_OK;
}

static ཧྲིལ tcl_has_attr(ClientData, Tcl_Interp *interp, ཧྲིལ argc, const var_na *argv[])
{
	ཧྲིལ i;
	bool mod_flag = false;
	དོན་ལུ (i = 1; i < argc; i++) {
		FLAG(mod)
		bha_nga;
	}

	གལ་སྲིད ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	གལ་སྲིད (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	གལ་སྲིད (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	གལ་སྲིད (mod_flag) {
		obj = mod;
	} དེ་མིན {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		གལ་སྲིད (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		གལ་སྲིད (!obj)
			obj = mod->cell(obj_twine);
		གལ་སྲིད (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	གལ་སྲིད (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	ལོག TCL_OK;
}

static ཧྲིལ tcl_set_attr(ClientData, Tcl_Interp *interp, ཧྲིལ objc, Tcl_Obj *const objv[])
{
	ཧྲིལ i;
	bool mod_flag = false, string_flag = false, bool_flag = false;
	bool true_flag = false, false_flag = false, sint_flag = false, uint_flag = false;
	དོན་ལུ (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(true)
		FLAG2(false)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(bool)
		bha_nga;
	}

	གལ་སྲིད ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	གལ་སྲིད (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	གལ་སྲིད (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	གལ་སྲིད (mod_flag) {
		obj = mod;
	} དེ་མིན {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		གལ་སྲིད (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		གལ་སྲིད (!obj)
			obj = mod->cell(obj_twine);
		གལ་སྲིད (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	གལ་སྲིད (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	གལ་སྲིད (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} དེ་མིན གལ་སྲིད (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int value_mp;

		གལ་སྲིད (Tcl_TakeBignumFromObj(interp, objv[i++], &value_mp))
			ERROR("non-integral value")

		གལ་སྲིད (!mp_int_to_const(&value_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		གལ་སྲིད (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			གལ་སྲིད (const_.size() < 32)
				const_.exts(32);
		} དེ་མིན {
			གལ་སྲིད (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} དེ་མིན གལ་སྲིད (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} དེ་མིན གལ་སྲིད (true_flag) {
		obj->set_bool_attribute(attr_twine, true);
	} དེ་མིན གལ་སྲིད (false_flag) {
		obj->set_bool_attribute(attr_twine, false);
	} དེ་མིན {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ལོག TCL_OK;
}

static ཧྲིལ tcl_get_param(ClientData, Tcl_Interp *interp, ཧྲིལ argc, const var_na *argv[])
{
	ཧྲིལ i;
	bool string_flag = false;
	bool int_flag = false, sint_flag = false, uint_flag = false;
	དོན་ལུ (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(ཧྲིལ)
		FLAG(sint)
		FLAG(uint)
		bha_nga;
	}

	གལ་སྲིད ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	གལ་སྲིད (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	གལ་སྲིད (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	གལ་སྲིད (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	const RTLIL::Const &value = cell->getParam(param_twine);

	གལ་སྲིད (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(value.decode_string().c_str(), -1));
	} དེ་མིན གལ་སྲིད (int_flag || uint_flag || sint_flag) {
		mp_int value_mp;
		གལ་སྲིད (!const_to_mp_int(value, &value_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&value_mp));
	} དེ་མིན {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(value.as_string().c_str(), -1));
	}
	ལོག TCL_OK;
}

static ཧྲིལ tcl_set_param(ClientData, Tcl_Interp *interp, ཧྲིལ objc, Tcl_Obj *const objv[])
{
	ཧྲིལ i;
	bool string_flag = false, sint_flag = false, uint_flag = false;
	དོན་ལུ (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		bha_nga;
	}

	གལ་སྲིད ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	གལ་སྲིད (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	གལ་སྲིད (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	གལ་སྲིད (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} དེ་མིན གལ་སྲིད (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int value_mp;

		གལ་སྲིད (Tcl_TakeBignumFromObj(interp, objv[i++], &value_mp))
			ERROR("non-integral value")

		གལ་སྲིད (!mp_int_to_const(&value_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		གལ་སྲིད (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			གལ་སྲིད (const_.size() < 32)
				const_.exts(32);
		} དེ་མིན {
			གལ་སྲིད (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} དེ་མིན {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ལོག TCL_OK;
}

ཧྲིལ yosys_tcl_interp_init(Tcl_Interp *interp)
{
	གལ་སྲིད (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_ErrnoMsg(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, NULL, NULL);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, NULL, NULL);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, NULL, NULL);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, NULL, NULL);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, NULL, NULL);

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

	ལོག TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
