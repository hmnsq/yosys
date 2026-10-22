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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#ಅನ್ತರ್ಭಾವ <tcl.h>
#ಅನ್ತರ್ಭಾವ <tclTomMath.h>
#ಅನ್ತರ್ಭಾವ <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
ತಾರ್ಕಿಕ yosys_tcl_repl_active = ಅಸತ್ಯ;

ಶೂನ್ಯ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = ಸತ್ಯ;
}

ಸ್ಥಿರ Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ಧ್ರುವ json11::Json &json)
{
	ಯದಿ (json.is_null())
		ಪ್ರತ್ಯಾಗಮ Tcl_NewStringObj("null", 4);
	ಅನ್ಯಥಾ ಯದಿ (json.is_string()) {
		auto string = json.string_ಮೌಲ್ಯ();
		ಪ್ರತ್ಯಾಗಮ Tcl_NewStringObj(string.data(), string.size());
	} ಅನ್ಯಥಾ ಯದಿ (json.is_ಸಂಖ್ಯೆ()) {
		ದ್ವಿಗುಣ ಮೌಲ್ಯ = json.ಸಂಖ್ಯೆ_ಮೌಲ್ಯ();
		ದ್ವಿಗುಣ round_val = std::nearbyint(ಮೌಲ್ಯ);
		ಯದಿ (std::isfinite(round_val) && ಮೌಲ್ಯ == round_val && ಮೌಲ್ಯ >= LONG_MIN && ಮೌಲ್ಯ < -ದ್ವಿಗುಣ(LONG_MIN))
			ಪ್ರತ್ಯಾಗಮ Tcl_NewLongObj((long)round_val);
		ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ Tcl_NewDoubleObj(ಮೌಲ್ಯ);
	} ಅನ್ಯಥಾ ಯದಿ (json.is_bool()) {
		ಪ್ರತ್ಯಾಗಮ Tcl_NewBooleanObj(json.bool_ಮೌಲ್ಯ());
	} ಅನ್ಯಥಾ ಯದಿ (json.is_array()) {
		auto list = json.array_ವಸ್ತುಗಳು();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		ಪ್ರತಿ (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		ಪ್ರತ್ಯಾಗಮ result;
	} ಅನ್ಯಥಾ ಯದಿ (json.is_object()) {
		auto map = json.object_ವಸ್ತುಗಳು();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		ಪ್ರತಿ (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		ಪ್ರತ್ಯಾಗಮ result;
	} ಅನ್ಯಥಾ {
		log_abort();
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcl_yosys_cmd(ClientData, Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ argc, ಧ್ರುವ ವರ್ಣ *argv[])
{
	std::vector<std::string> args;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < argc; i++)
		args.ತಳ್ಳು_back(argv[i]);

	ಯದಿ (args.size() >= 1 && args[0] == "-import") {
		ಪ್ರತಿ (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			ಯದಿ (tcl_command_name == "proc")
				tcl_command_name = "procs";
			ಅನ್ಯಥಾ ಯದಿ (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			ಯದಿ (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} ಅನ್ಯಥಾ {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ಪ್ರತ್ಯಾಗಮ TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	ತಾರ್ಕಿಕ in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	ಯತ್ನ {
		ಯದಿ (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} ಅನ್ಯಥಾ {
			Pass::call(yosys_get_design(), args);
		}
	} ಗೃಹಾಣ (log_cmd_error_exception) {
		ಯದಿ (in_repl) {
			auto design = yosys_get_design();
			ಯಾವತ್ (design->selection_ರಾಶಿ.size() > 1)
				design->ತೆಗೆ_selection();
			log_reset_ರಾಶಿ();
		}
		Tcl_SetResult(interp, (ವರ್ಣ *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		ಪ್ರತ್ಯಾಗಮ TCL_ERROR;
	} ಗೃಹಾಣ (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	ಯದಿ (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		ಯದಿ (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} ಅನ್ಯಥಾ
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} ಅನ್ಯಥಾ ಯದಿ ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

#define FLAG(name) \
	ಯದಿ (!strcmp(argv[i], "-" #name)) { \
		name##_flag = ಸತ್ಯ; \
		ಅನುವರ್ತ; \
	} \

#define FLAG2(name) \
	ಯದಿ (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = ಸತ್ಯ; \
		ಅನುವರ್ತ; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (ವರ್ಣ *)(str), TCL_STATIC); \
		ಪ್ರತ್ಯಾಗಮ TCL_ERROR; \
	}

ತಾರ್ಕಿಕ const_to_mp_int(ಧ್ರುವ Const &a, mp_int *b, ತಾರ್ಕಿಕ force_signed, ತಾರ್ಕಿಕ force_unsigned)
{
	ಯದಿ (!a.is_fully_def())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಯದಿ (mp_init(b))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ತಾರ್ಕಿಕ negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = a.size() - 1; i >= 0; i--) {
		ಯದಿ (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		ಯದಿ ((a[i] == RTLIL::S1) ^ negative) {
			ಯದಿ (mp_add_d(b, 1, b)) {
				mp_clear(b);
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}
	}

	ಯದಿ (negative) {
		ಯದಿ (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ mp_int_to_const(mp_int *a, Const &b, ತಾರ್ಕಿಕ is_signed)
{
	ತಾರ್ಕಿಕ negative = (mp_cmp_d(a, 0) == MP_LT);
	ಯದಿ (negative && !is_signed)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಯದಿ (negative) {
		ಯದಿ (mp_neg(a, a) != MP_OKAY)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಯದಿ (mp_sub_d(a, 1, a) != MP_OKAY)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	std::vector<unsigned ವರ್ಣ> buf;
	buf.resize(mp_unsigned_bin_size(a));
	ಯದಿ (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	Const::Builder b_bits(mp_ಎಣಿಕೆ_bits(a) + is_signed);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mp_ಎಣಿಕೆ_bits(a);) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < 8 && i < mp_ಎಣಿಕೆ_bits(a); j++, i++) {
			ತಾರ್ಕಿಕ bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.ತಳ್ಳು_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.ತೆಗೆ_back();
	}

	ಯದಿ (is_signed) {
		b_bits.ತಳ್ಳು_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcl_get_attr(ClientData, Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ argc, ಧ್ರುವ ವರ್ಣ *argv[])
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ತಾರ್ಕಿಕ mod_flag = ಅಸತ್ಯ, string_flag = ಅಸತ್ಯ, bool_flag = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ int_flag = ಅಸತ್ಯ, sint_flag = ಅಸತ್ಯ, uint_flag = ಅಸತ್ಯ;
	ಪ್ರತಿ (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(ಪೂರ್ಣಾಙ್ಕ)
		FLAG(sint)
		FLAG(uint)
		FLAG(ತಾರ್ಕಿಕ)
		ಭಙ್ಗ;
	}

	ಯದಿ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ಯದಿ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಯದಿ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ಯದಿ (mod_flag) {
		obj = mod;
	} ಅನ್ಯಥಾ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ಯದಿ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ಯದಿ (!obj)
			obj = mod->cell(obj_twine);
		ಯದಿ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ಯದಿ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	ಯದಿ (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} ಅನ್ಯಥಾ ಯದಿ (int_flag || uint_flag || sint_flag) {
		ಯದಿ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &ಮೌಲ್ಯ = obj->attributes.at(attr_twine);

		mp_int ಮೌಲ್ಯ_mp;
		ಯದಿ (!const_to_mp_int(ಮೌಲ್ಯ, &ಮೌಲ್ಯ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&ಮೌಲ್ಯ_mp));
	} ಅನ್ಯಥಾ ಯದಿ (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} ಅನ್ಯಥಾ {
		ಯದಿ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcl_has_attr(ClientData, Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ argc, ಧ್ರುವ ವರ್ಣ *argv[])
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ತಾರ್ಕಿಕ mod_flag = ಅಸತ್ಯ;
	ಪ್ರತಿ (i = 1; i < argc; i++) {
		FLAG(mod)
		ಭಙ್ಗ;
	}

	ಯದಿ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ಯದಿ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಯದಿ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ಯದಿ (mod_flag) {
		obj = mod;
	} ಅನ್ಯಥಾ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ಯದಿ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ಯದಿ (!obj)
			obj = mod->cell(obj_twine);
		ಯದಿ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ಯದಿ (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcl_set_attr(ClientData, Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ objc, Tcl_Obj *ಧ್ರುವ objv[])
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ತಾರ್ಕಿಕ mod_flag = ಅಸತ್ಯ, string_flag = ಅಸತ್ಯ, bool_flag = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ true_flag = ಅಸತ್ಯ, false_flag = ಅಸತ್ಯ, sint_flag = ಅಸತ್ಯ, uint_flag = ಅಸತ್ಯ;
	ಪ್ರತಿ (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(ಸತ್ಯ)
		FLAG2(ಅಸತ್ಯ)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(ತಾರ್ಕಿಕ)
		ಭಙ್ಗ;
	}

	ಯದಿ ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	ಯದಿ (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಯದಿ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ಯದಿ (mod_flag) {
		obj = mod;
	} ಅನ್ಯಥಾ {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ಯದಿ (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ಯದಿ (!obj)
			obj = mod->cell(obj_twine);
		ಯದಿ (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ಯದಿ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	ಯದಿ (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} ಅನ್ಯಥಾ ಯದಿ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ಮೌಲ್ಯ_mp;

		ಯದಿ (Tcl_TakeBignumFromObj(interp, objv[i++], &ಮೌಲ್ಯ_mp))
			ERROR("non-integral value")

		ಯದಿ (!mp_int_to_const(&ಮೌಲ್ಯ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ಯದಿ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ಯದಿ (const_.size() < 32)
				const_.exts(32);
		} ಅನ್ಯಥಾ {
			ಯದಿ (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} ಅನ್ಯಥಾ ಯದಿ (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} ಅನ್ಯಥಾ ಯದಿ (true_flag) {
		obj->set_bool_attribute(attr_twine, ಸತ್ಯ);
	} ಅನ್ಯಥಾ ಯದಿ (false_flag) {
		obj->set_bool_attribute(attr_twine, ಅಸತ್ಯ);
	} ಅನ್ಯಥಾ {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcl_get_param(ClientData, Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ argc, ಧ್ರುವ ವರ್ಣ *argv[])
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ತಾರ್ಕಿಕ string_flag = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ int_flag = ಅಸತ್ಯ, sint_flag = ಅಸತ್ಯ, uint_flag = ಅಸತ್ಯ;
	ಪ್ರತಿ (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(ಪೂರ್ಣಾಙ್ಕ)
		FLAG(sint)
		FLAG(uint)
		ಭಙ್ಗ;
	}

	ಯದಿ ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಯದಿ (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	ಯದಿ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	ಯದಿ (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ಧ್ರುವ RTLIL::Const &ಮೌಲ್ಯ = cell->getParam(param_twine);

	ಯದಿ (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(ಮೌಲ್ಯ.decode_string().c_str(), -1));
	} ಅನ್ಯಥಾ ಯದಿ (int_flag || uint_flag || sint_flag) {
		mp_int ಮೌಲ್ಯ_mp;
		ಯದಿ (!const_to_mp_int(ಮೌಲ್ಯ, &ಮೌಲ್ಯ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&ಮೌಲ್ಯ_mp));
	} ಅನ್ಯಥಾ {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(ಮೌಲ್ಯ.as_string().c_str(), -1));
	}
	ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tcl_set_param(ClientData, Tcl_Interp *interp, ಪೂರ್ಣಾಙ್ಕ objc, Tcl_Obj *ಧ್ರುವ objv[])
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ತಾರ್ಕಿಕ string_flag = ಅಸತ್ಯ, sint_flag = ಅಸತ್ಯ, uint_flag = ಅಸತ್ಯ;
	ಪ್ರತಿ (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		ಭಙ್ಗ;
	}

	ಯದಿ ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಯದಿ (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	ಯದಿ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	ಯದಿ (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} ಅನ್ಯಥಾ ಯದಿ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ಮೌಲ್ಯ_mp;

		ಯದಿ (Tcl_TakeBignumFromObj(interp, objv[i++], &ಮೌಲ್ಯ_mp))
			ERROR("non-integral value")

		ಯದಿ (!mp_int_to_const(&ಮೌಲ್ಯ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ಯದಿ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ಯದಿ (const_.size() < 32)
				const_.exts(32);
		} ಅನ್ಯಥಾ {
			ಯದಿ (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} ಅನ್ಯಥಾ {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ಪ್ರತ್ಯಾಗಮ TCL_OK;
}

ಪೂರ್ಣಾಙ್ಕ yosys_tcl_interp_init(Tcl_Interp *interp)
{
	ಯದಿ (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_ಸಂದೇಶ(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ರಿಕ್ತ, ರಿಕ್ತ);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ರಿಕ್ತ, ರಿಕ್ತ);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ರಿಕ್ತ, ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, ರಿಕ್ತ, ರಿಕ್ತ);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ರಿಕ್ತ, ರಿಕ್ತ);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, ರಿಕ್ತ, ರಿಕ್ತ);

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

	ಪ್ರತ್ಯಾಗಮ TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
