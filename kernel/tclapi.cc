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
	ᬬᬾᬦ᭄ (json.is_null())
		ᬯᬮᬶ Tcl_NewStringObj("null", 4);
	ᬮᬶᬬ ᬬᬾᬦ᭄ (json.is_string()) {
		auto string = json.string_value();
		ᬯᬮᬶ Tcl_NewStringObj(string.data(), string.size());
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (json.is_number()) {
		dviguu_na value = json.number_value();
		dviguu_na round_val = std::nearbyint(value);
		ᬬᬾᬦ᭄ (std::isfinite(round_val) && value == round_val && value >= LONG_MIN && value < -dviguu_na(LONG_MIN))
			ᬯᬮᬶ Tcl_NewLongObj((long)round_val);
		ᬮᬶᬬ
			ᬯᬮᬶ Tcl_NewDoubleObj(value);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (json.is_bool()) {
		ᬯᬮᬶ Tcl_NewBooleanObj(json.bool_value());
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (json.is_array()) {
		auto list = json.array_items();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		for (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		ᬯᬮᬶ result;
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (json.is_object()) {
		auto map = json.object_items();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		for (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		ᬯᬮᬶ result;
	} ᬮᬶᬬ {
		log_abort();
	}
}

static puur_naa_nka tcl_yosys_cmd(ClientData, Tcl_Interp *interp, puur_naa_nka argc, const var_na *argv[])
{
	std::vector<std::string> args;
	for (puur_naa_nka i = 1; i < argc; i++)
		args.push_back(argv[i]);

	ᬬᬾᬦ᭄ (args.size() >= 1 && args[0] == "-import") {
		for (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			ᬬᬾᬦ᭄ (tcl_command_name == "proc")
				tcl_command_name = "procs";
			ᬮᬶᬬ ᬬᬾᬦ᭄ (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			ᬬᬾᬦ᭄ (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} ᬮᬶᬬ {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ᬯᬮᬶ TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	bool in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	try {
		ᬬᬾᬦ᭄ (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} ᬮᬶᬬ {
			Pass::call(yosys_get_design(), args);
		}
	} gr_haa_na (log_cmd_error_exception) {
		ᬬᬾᬦ᭄ (in_repl) {
			auto design = yosys_get_design();
			ᬲᭂᬤᭂᬮ (design->selection_stack.size() > 1)
				design->pop_selection();
			log_reset_stack();
		}
		Tcl_SetResult(interp, (var_na *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		ᬯᬮᬶ TCL_ERROR;
	} gr_haa_na (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	ᬬᬾᬦ᭄ (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		ᬬᬾᬦ᭄ (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} ᬮᬶᬬ
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	ᬯᬮᬶ TCL_OK;
}

#define FLAG(name) \
	ᬬᬾᬦ᭄ (!strcmp(argv[i], "-" #name)) { \
		name##_flag = true; \
		continue; \
	} \

#define FLAG2(name) \
	ᬬᬾᬦ᭄ (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = true; \
		continue; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (var_na *)(str), TCL_STATIC); \
		ᬯᬮᬶ TCL_ERROR; \
	}

bool const_to_mp_int(const Const &a, mp_int *b, bool force_signed, bool force_unsigned)
{
	ᬬᬾᬦ᭄ (!a.is_fully_def())
		ᬯᬮᬶ false;

	ᬬᬾᬦ᭄ (mp_init(b))
		ᬯᬮᬶ false;

	bool negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	for (puur_naa_nka i = a.size() - 1; i >= 0; i--) {
		ᬬᬾᬦ᭄ (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ᬯᬮᬶ false;
		}

		ᬬᬾᬦ᭄ ((a[i] == RTLIL::S1) ^ negative) {
			ᬬᬾᬦ᭄ (mp_add_d(b, 1, b)) {
				mp_clear(b);
				ᬯᬮᬶ false;
			}
		}
	}

	ᬬᬾᬦ᭄ (negative) {
		ᬬᬾᬦ᭄ (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ᬯᬮᬶ false;
		}
	}

	ᬯᬮᬶ true;
}

bool mp_int_to_const(mp_int *a, Const &b, bool is_signed)
{
	bool negative = (mp_cmp_d(a, 0) == MP_LT);
	ᬬᬾᬦ᭄ (negative && !is_signed)
		ᬯᬮᬶ false;

	ᬬᬾᬦ᭄ (negative) {
		ᬬᬾᬦ᭄ (mp_neg(a, a) != MP_OKAY)
			ᬯᬮᬶ false;
		ᬬᬾᬦ᭄ (mp_sub_d(a, 1, a) != MP_OKAY)
			ᬯᬮᬶ false;
	}

	std::vector<unsigned var_na> buf;
	buf.resize(mp_unsigned_bin_size(a));
	ᬬᬾᬦ᭄ (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		ᬯᬮᬶ false;

	Const::Builder b_bits(mp_count_bits(a) + is_signed);
	for (puur_naa_nka i = 0; i < mp_count_bits(a);) {
		for (puur_naa_nka j = 0; j < 8 && i < mp_count_bits(a); j++, i++) {
			bool bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.push_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.pop_back();
	}

	ᬬᬾᬦ᭄ (is_signed) {
		b_bits.push_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	ᬯᬮᬶ true;
}

static puur_naa_nka tcl_get_attr(ClientData, Tcl_Interp *interp, puur_naa_nka argc, const var_na *argv[])
{
	puur_naa_nka i;
	bool mod_flag = false, string_flag = false, bool_flag = false;
	bool int_flag = false, sint_flag = false, uint_flag = false;
	for (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(puur_naa_nka)
		FLAG(sint)
		FLAG(uint)
		FLAG(bool)
		bha_nga;
	}

	ᬬᬾᬦ᭄ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ᬬᬾᬦ᭄ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ᬬᬾᬦ᭄ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ᬬᬾᬦ᭄ (mod_flag) {
		obj = mod;
	} ᬮᬶᬬ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->cell(obj_twine);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ᬬᬾᬦ᭄ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	ᬬᬾᬦ᭄ (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (int_flag || uint_flag || sint_flag) {
		ᬬᬾᬦ᭄ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &value = obj->attributes.at(attr_twine);

		mp_int value_mp;
		ᬬᬾᬦ᭄ (!const_to_mp_int(value, &value_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&value_mp));
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} ᬮᬶᬬ {
		ᬬᬾᬦ᭄ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	ᬯᬮᬶ TCL_OK;
}

static puur_naa_nka tcl_has_attr(ClientData, Tcl_Interp *interp, puur_naa_nka argc, const var_na *argv[])
{
	puur_naa_nka i;
	bool mod_flag = false;
	for (i = 1; i < argc; i++) {
		FLAG(mod)
		bha_nga;
	}

	ᬬᬾᬦ᭄ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ᬬᬾᬦ᭄ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ᬬᬾᬦ᭄ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ᬬᬾᬦ᭄ (mod_flag) {
		obj = mod;
	} ᬮᬶᬬ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->cell(obj_twine);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ᬬᬾᬦ᭄ (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	ᬯᬮᬶ TCL_OK;
}

static puur_naa_nka tcl_set_attr(ClientData, Tcl_Interp *interp, puur_naa_nka objc, Tcl_Obj *const objv[])
{
	puur_naa_nka i;
	bool mod_flag = false, string_flag = false, bool_flag = false;
	bool true_flag = false, false_flag = false, sint_flag = false, uint_flag = false;
	for (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(true)
		FLAG2(false)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(bool)
		bha_nga;
	}

	ᬬᬾᬦ᭄ ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	ᬬᬾᬦ᭄ (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ᬬᬾᬦ᭄ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ᬬᬾᬦ᭄ (mod_flag) {
		obj = mod;
	} ᬮᬶᬬ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->cell(obj_twine);
		ᬬᬾᬦ᭄ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ᬬᬾᬦ᭄ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	ᬬᬾᬦ᭄ (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int value_mp;

		ᬬᬾᬦ᭄ (Tcl_TakeBignumFromObj(interp, objv[i++], &value_mp))
			ERROR("non-integral value")

		ᬬᬾᬦ᭄ (!mp_int_to_const(&value_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ᬬᬾᬦ᭄ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ᬬᬾᬦ᭄ (const_.size() < 32)
				const_.exts(32);
		} ᬮᬶᬬ {
			ᬬᬾᬦ᭄ (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (true_flag) {
		obj->set_bool_attribute(attr_twine, true);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (false_flag) {
		obj->set_bool_attribute(attr_twine, false);
	} ᬮᬶᬬ {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ᬯᬮᬶ TCL_OK;
}

static puur_naa_nka tcl_get_param(ClientData, Tcl_Interp *interp, puur_naa_nka argc, const var_na *argv[])
{
	puur_naa_nka i;
	bool string_flag = false;
	bool int_flag = false, sint_flag = false, uint_flag = false;
	for (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(puur_naa_nka)
		FLAG(sint)
		FLAG(uint)
		bha_nga;
	}

	ᬬᬾᬦ᭄ ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ᬬᬾᬦ᭄ (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	ᬬᬾᬦ᭄ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	ᬬᬾᬦ᭄ (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	const RTLIL::Const &value = cell->getParam(param_twine);

	ᬬᬾᬦ᭄ (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(value.decode_string().c_str(), -1));
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (int_flag || uint_flag || sint_flag) {
		mp_int value_mp;
		ᬬᬾᬦ᭄ (!const_to_mp_int(value, &value_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&value_mp));
	} ᬮᬶᬬ {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(value.as_string().c_str(), -1));
	}
	ᬯᬮᬶ TCL_OK;
}

static puur_naa_nka tcl_set_param(ClientData, Tcl_Interp *interp, puur_naa_nka objc, Tcl_Obj *const objv[])
{
	puur_naa_nka i;
	bool string_flag = false, sint_flag = false, uint_flag = false;
	for (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		bha_nga;
	}

	ᬬᬾᬦ᭄ ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ᬬᬾᬦ᭄ (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	ᬬᬾᬦ᭄ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	ᬬᬾᬦ᭄ (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int value_mp;

		ᬬᬾᬦ᭄ (Tcl_TakeBignumFromObj(interp, objv[i++], &value_mp))
			ERROR("non-integral value")

		ᬬᬾᬦ᭄ (!mp_int_to_const(&value_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ᬬᬾᬦ᭄ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ᬬᬾᬦ᭄ (const_.size() < 32)
				const_.exts(32);
		} ᬮᬶᬬ {
			ᬬᬾᬦ᭄ (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} ᬮᬶᬬ {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ᬯᬮᬶ TCL_OK;
}

puur_naa_nka yosys_tcl_interp_init(Tcl_Interp *interp)
{
	ᬬᬾᬦ᭄ (Tcl_Init(interp)!=TCL_OK)
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

	ᬯᬮᬶ TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
