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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
#ಅನ್ತರ್ಭಾವ <tcl.h>
#ಅನ್ತರ್ಭಾವ <tclTomMath.h>
#ಅನ್ತರ್ಭಾವ <tclTomMathDecls.h>
#ಆದರೆ_ಅಂತ್ಯ

YOSYS_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
ತಾರ್ಕಿಕ yosys_tcl_repl_ಸಕ್ರಿಯ = ಸುಳ್ಳು;

ಶೂನ್ಯ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_ಸಕ್ರಿಯ = ನಿಜ;
}

ಸ್ಥಿರ Tcl_Obj *json_ಗಮ್ಯ_tcl(Tcl_Interp *interp, ಸ್ಥಿರಾಂಕ json11::Json &json)
{
	ಆದರೆ (json.is_null())
		ಹಿಂತಿರುಗಿ Tcl_NewStringObj("null", 4);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (json.is_string()) {
		ಸ್ವಯಂಚಾಲಿತ string = json.string_ಮೌಲ್ಯ();
		ಹಿಂತಿರುಗಿ Tcl_NewStringObj(string.ದತ್ತಾಂಶ(), string.ಗಾತ್ರ_2());
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (json.is_ಸಂಖ್ಯೆ()) {
		ದ್ವಿಗುಣ ಮೌಲ್ಯ = json.ಸಂಖ್ಯೆ_ಮೌಲ್ಯ();
		ದ್ವಿಗುಣ ಸುತ್ತು_val = std::nearbyint(ಮೌಲ್ಯ);
		ಆದರೆ (std::isfinite(ಸುತ್ತು_val) && ಮೌಲ್ಯ == ಸುತ್ತು_val && ಮೌಲ್ಯ >= LONG_MIN && ಮೌಲ್ಯ < -ದ್ವಿಗುಣ(LONG_MIN))
			ಹಿಂತಿರುಗಿ Tcl_NewLongObj((ದೀರ್ಘ)ಸುತ್ತು_val);
		ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ Tcl_NewDoubleObj(ಮೌಲ್ಯ);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (json.is_bool()) {
		ಹಿಂತಿರುಗಿ Tcl_NewBooleanObj(json.bool_ಮೌಲ್ಯ());
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (json.is_array()) {
		ಸ್ವಯಂಚಾಲಿತ list = json.array_ವಸ್ತುಗಳು();
		Tcl_Obj *ಫಲಿತಾಂಶ = Tcl_NewListObj(list.ಗಾತ್ರ_2(), ಶೂನ್ಯಸೂಚಕ);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಅಂಶ : list)
			Tcl_ListObjAppendElement(interp, ಫಲಿತಾಂಶ, json_ಗಮ್ಯ_tcl(interp, ಅಂಶ));
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (json.is_object()) {
		ಸ್ವಯಂಚಾಲಿತ map = json.object_ವಸ್ತುಗಳು();
		Tcl_Obj *ಫಲಿತಾಂಶ = Tcl_NewListObj(map.ಗಾತ್ರ_2() * 2, ಶೂನ್ಯಸೂಚಕ);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಅಂಶ : map) {
			Tcl_ListObjAppendElement(interp, ಫಲಿತಾಂಶ, Tcl_NewStringObj(ಅಂಶ.ಮೊದಲ.ದತ್ತಾಂಶ(), ಅಂಶ.ಮೊದಲ.ಗಾತ್ರ_2()));
			Tcl_ListObjAppendElement(interp, ಫಲಿತಾಂಶ, json_ಗಮ್ಯ_tcl(interp, ಅಂಶ.ಎರಡನೇ));
		}
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_abort();
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcl_yosys_cmd(Client_ದತ್ತಾಂಶ, Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *argv[])
{
	std::ಸದಿಶ<std::string> ವಾದಗಳು_2;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < argc; i++)
		ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(argv[i]);

	ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() >= 1 && ವಾದಗಳು_2[0] == "-import") {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : pass_ರಿಜಿಸ್ಟರ್) {
			std::string tcl_command_name = it.ಮೊದಲ;
			ಆದರೆ (tcl_command_name == "proc")
				tcl_command_name = "procs";
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			ಆದರೆ (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				ಲಘುಗಣಕ("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.ಮೊದಲ);
			} ಇಲ್ಲದಿದ್ದರೆ {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.ಮೊದಲ);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ಹಿಂತಿರುಗಿ TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	ತಾರ್ಕಿಕ ಒಳ_repl = yosys_tcl_repl_ಸಕ್ರಿಯ;
	ಸ್ವಯಂಚಾಲಿತ guard = logger().error_throw_scope();
	ಪ್ರಯತ್ನಿಸು {
		ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 1) {
			Pass::call(yosys_get_design(), ವಾದಗಳು_2[0]);
		} ಇಲ್ಲದಿದ್ದರೆ {
			Pass::call(yosys_get_design(), ವಾದಗಳು_2);
		}
	} ಹಿಡಿ (log_cmd_error_exception) {
		ಆದರೆ (ಒಳ_repl) {
			ಸ್ವಯಂಚಾಲಿತ design = yosys_get_design();
			ಆಗ (design->selection_ರಾಶಿ.ಗಾತ್ರ_2() > 1)
				design->ತೆಗೆ_selection();
			log_reset_ರಾಶಿ();
		}
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, (ಅಕ್ಷರ *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_ಸಕ್ರಿಯ = ಒಳ_repl;
		ಹಿಂತಿರುಗಿ TCL_ERROR;
	} ಹಿಡಿ (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_ಸಕ್ರಿಯ = ಒಳ_repl;

	ಸ್ವಯಂಚಾಲಿತ &scratchpad = yosys_get_design()->scratchpad;
	ಸ್ವಯಂಚಾಲಿತ ಫಲಿತಾಂಶ = scratchpad.ಕಂಡುಹಿಡಿ("result.json");
	ಆದರೆ (ಫಲಿತಾಂಶ != scratchpad.ಕೊನೆ()) {
		std::string err;
		ಸ್ವಯಂಚಾಲಿತ json = json11::Json::parse(ಫಲಿತಾಂಶ->ಎರಡನೇ, err);
		ಆದರೆ (err.ಖಾಲಿಗಳು()) {
			Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, json_ಗಮ್ಯ_tcl(interp, json));
		} ಇಲ್ಲದಿದ್ದರೆ
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಫಲಿತಾಂಶ = scratchpad.ಕಂಡುಹಿಡಿ("result.string")) != scratchpad.ಕೊನೆ()) {
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewStringObj(ಫಲಿತಾಂಶ->ಎರಡನೇ.ದತ್ತಾಂಶ(), ಫಲಿತಾಂಶ->ಎರಡನೇ.ಗಾತ್ರ_2()));
	}

	ಹಿಂತಿರುಗಿ TCL_OK;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FLAG(name) \
	ಆದರೆ (!strcmp(argv[i], "-" #name)) { \
		name##_flag = ನಿಜ; \
		ಮುಂದುವರಿ; \
	} \

#ವ್ಯಾಖ್ಯಾನಿಸು FLAG2(name) \
	ಆದರೆ (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = ನಿಜ; \
		ಮುಂದುವರಿ; \
	} \

#ವ್ಯಾಖ್ಯಾನಿಸು ERROR(str) \
	{ \
		Tcl_ಗಣ_ಫಲಿತಾಂಶ(interp, (ಅಕ್ಷರ *)(str), TCL_STATIC); \
		ಹಿಂತಿರುಗಿ TCL_ERROR; \
	}

ತಾರ್ಕಿಕ const_ಗಮ್ಯ_mp_int(ಸ್ಥಿರಾಂಕ Const &a, mp_int *b, ತಾರ್ಕಿಕ force_signed, ತಾರ್ಕಿಕ force_unsigned)
{
	ಆದರೆ (!a.is_fully_def())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಆದರೆ (mp_init(b))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ತಾರ್ಕಿಕ negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.ಖಾಲಿಗಳು() && (a.ಹಿಂಭಾಗ() == RTLIL::S1);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = a.ಗಾತ್ರ_2() - 1; i >= 0; i--) {
		ಆದರೆ (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		ಆದರೆ ((a[i] == RTLIL::S1) ^ negative) {
			ಆದರೆ (mp_ಕೂಡಿಸು_d(b, 1, b)) {
				mp_clear(b);
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
		}
	}

	ಆದರೆ (negative) {
		ಆದರೆ (mp_ಕೂಡಿಸು_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ mp_int_ಗಮ್ಯ_const(mp_int *a, Const &b, ತಾರ್ಕಿಕ is_signed)
{
	ತಾರ್ಕಿಕ negative = (mp_cmp_d(a, 0) == MP_LT);
	ಆದರೆ (negative && !is_signed)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಆದರೆ (negative) {
		ಆದರೆ (mp_neg(a, a) != MP_OKAY)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಆದರೆ (mp_sub_d(a, 1, a) != MP_OKAY)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	std::ಸದಿಶ<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ> buf;
	buf.resize(mp_unsigned_bin_ಗಾತ್ರ(a));
	ಆದರೆ (mp_ಗಮ್ಯ_unsigned_bin(a, buf.ದತ್ತಾಂಶ()) != MP_OKAY)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	Const::Builder b_ಬಿಟ್ಟುಗಳು(mp_ಎಣಿಕೆ_ಬಿಟ್ಟುಗಳು(a) + is_signed);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mp_ಎಣಿಕೆ_ಬಿಟ್ಟುಗಳು(a);) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < 8 && i < mp_ಎಣಿಕೆ_ಬಿಟ್ಟುಗಳು(a); j++, i++) {
			ತಾರ್ಕಿಕ bv = ((buf.ಹಿಂಭಾಗ() & (1 << j)) != 0) ^ negative;
			b_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.ತೆಗೆ_ಹಿಂಭಾಗ();
	}

	ಆದರೆ (is_signed) {
		b_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_ಬಿಟ್ಟುಗಳು.build();

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcl_get_attr(Client_ದತ್ತಾಂಶ, Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *argv[])
{
	ಪೂರ್ಣಾಂಕ i;
	ತಾರ್ಕಿಕ mod_flag = ಸುಳ್ಳು, string_flag = ಸುಳ್ಳು, bool_flag = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ int_flag = ಸುಳ್ಳು, sint_flag = ಸುಳ್ಳು, uint_flag = ಸುಳ್ಳು;
	ಗಾಗಿ (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(ಪೂರ್ಣಾಂಕ)
		FLAG(sint)
		FLAG(uint)
		FLAG(ತಾರ್ಕಿಕ)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ಆದರೆ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	ಸ್ವಯಂಚಾಲಿತ mod_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಆದರೆ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = ಶೂನ್ಯಸೂಚಕ;
	ಆದರೆ (mod_flag) {
		obj = mod;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಸ್ವಯಂಚಾಲಿತ obj_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(obj_id);
		obj = mod->wire(obj_twine);
		ಆದರೆ (!obj)
			obj = mod->memories.at(obj_twine, ಶೂನ್ಯಸೂಚಕ);
		ಆದರೆ (!obj)
			obj = mod->cell(obj_twine);
		ಆದರೆ (!obj)
			obj = mod->processes.at(obj_twine, ಶೂನ್ಯಸೂಚಕ);
	}

	ಆದರೆ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(attr_id);

	ಆದರೆ (string_flag) {
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (int_flag || uint_flag || sint_flag) {
		ಆದರೆ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &ಮೌಲ್ಯ = obj->ಗುಣಲಕ್ಷಣಗಳು.at(attr_twine);

		mp_int ಮೌಲ್ಯ_mp;
		ಆದರೆ (!const_ಗಮ್ಯ_mp_int(ಮೌಲ್ಯ, &ಮೌಲ್ಯ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewBignumObj(&ಮೌಲ್ಯ_mp));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bool_flag) {
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewStringObj(obj->ಗುಣಲಕ್ಷಣಗಳು.at(attr_twine).as_string().c_str(), -1));
	}

	ಹಿಂತಿರುಗಿ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcl_has_attr(Client_ದತ್ತಾಂಶ, Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *argv[])
{
	ಪೂರ್ಣಾಂಕ i;
	ತಾರ್ಕಿಕ mod_flag = ಸುಳ್ಳು;
	ಗಾಗಿ (i = 1; i < argc; i++) {
		FLAG(mod)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ಆದರೆ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	ಸ್ವಯಂಚಾಲಿತ mod_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಆದರೆ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = ಶೂನ್ಯಸೂಚಕ;
	ಆದರೆ (mod_flag) {
		obj = mod;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಸ್ವಯಂಚಾಲಿತ obj_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(obj_id);
		obj = mod->wire(obj_twine);
		ಆದರೆ (!obj)
			obj = mod->memories.at(obj_twine, ಶೂನ್ಯಸೂಚಕ);
		ಆದರೆ (!obj)
			obj = mod->cell(obj_twine);
		ಆದರೆ (!obj)
			obj = mod->processes.at(obj_twine, ಶೂನ್ಯಸೂಚಕ);
	}

	ಆದರೆ (!obj)
		ERROR("object not found")

	Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewStringObj(std::ಗಮ್ಯ_string(obj->has_attribute(yosys_design->twines().ಕಂಡುಹಿಡಿ(attr_id))).c_str(), -1));
	ಹಿಂತಿರುಗಿ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcl_ಗಣ_attr(Client_ದತ್ತಾಂಶ, Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ objc, Tcl_Obj *ಸ್ಥಿರಾಂಕ objv[])
{
	ಪೂರ್ಣಾಂಕ i;
	ತಾರ್ಕಿಕ mod_flag = ಸುಳ್ಳು, string_flag = ಸುಳ್ಳು, bool_flag = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ true_flag = ಸುಳ್ಳು, false_flag = ಸುಳ್ಳು, sint_flag = ಸುಳ್ಳು, uint_flag = ಸುಳ್ಳು;
	ಗಾಗಿ (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(ನಿಜ)
		FLAG2(ಸುಳ್ಳು)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(ತಾರ್ಕಿಕ)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	ಆದರೆ (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	ಸ್ವಯಂಚಾಲಿತ mod_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಆದರೆ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = ಶೂನ್ಯಸೂಚಕ;
	ಆದರೆ (mod_flag) {
		obj = mod;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಸ್ವಯಂಚಾಲಿತ obj_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(obj_id);
		obj = mod->wire(obj_twine);
		ಆದರೆ (!obj)
			obj = mod->memories.at(obj_twine, ಶೂನ್ಯಸೂಚಕ);
		ಆದರೆ (!obj)
			obj = mod->cell(obj_twine);
		ಆದರೆ (!obj)
			obj = mod->processes.at(obj_twine, ಶೂನ್ಯಸೂಚಕ);
	}

	ಆದರೆ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().ಕೂಡಿಸು(attr_id);

	ಆದರೆ (string_flag) {
		obj->ಗಣ_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ಮೌಲ್ಯ_mp;

		ಆದರೆ (Tcl_TakeBignumFromObj(interp, objv[i++], &ಮೌಲ್ಯ_mp))
			ERROR("non-integral value")

		ಆದರೆ (!mp_int_ಗಮ್ಯ_const(&ಮೌಲ್ಯ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ಆದರೆ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ಆದರೆ (const_.ಗಾತ್ರ_2() < 32)
				const_.exts(32);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (const_.ಗಾತ್ರ_2() < 32)
				const_.extu(32);
		}

		obj->ಗುಣಲಕ್ಷಣಗಳು[attr_twine] = const_;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bool_flag) {
		obj->ಗಣ_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (true_flag) {
		obj->ಗಣ_bool_attribute(attr_twine, ನಿಜ);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (false_flag) {
		obj->ಗಣ_bool_attribute(attr_twine, ಸುಳ್ಳು);
	} ಇಲ್ಲದಿದ್ದರೆ {
		obj->ಗುಣಲಕ್ಷಣಗಳು[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ಹಿಂತಿರುಗಿ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcl_get_param(Client_ದತ್ತಾಂಶ, Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ argc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *argv[])
{
	ಪೂರ್ಣಾಂಕ i;
	ತಾರ್ಕಿಕ string_flag = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ int_flag = ಸುಳ್ಳು, sint_flag = ಸುಳ್ಳು, uint_flag = ಸುಳ್ಳು;
	ಗಾಗಿ (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(ಪೂರ್ಣಾಂಕ)
		FLAG(sint)
		FLAG(uint)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	ಸ್ವಯಂಚಾಲಿತ mod_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಆದರೆ (!mod)
		ERROR("module not found")

	ಸ್ವಯಂಚಾಲಿತ cell_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(cell_id);
	Cell* cell = mod->cell(cell_twine);
	ಆದರೆ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(param_id);
	ಆದರೆ (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ಸ್ಥಿರಾಂಕ RTLIL::Const &ಮೌಲ್ಯ = cell->getParam(param_twine);

	ಆದರೆ (string_flag) {
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewStringObj(ಮೌಲ್ಯ.decode_string().c_str(), -1));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (int_flag || uint_flag || sint_flag) {
		mp_int ಮೌಲ್ಯ_mp;
		ಆದರೆ (!const_ಗಮ್ಯ_mp_int(ಮೌಲ್ಯ, &ಮೌಲ್ಯ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewBignumObj(&ಮೌಲ್ಯ_mp));
	} ಇಲ್ಲದಿದ್ದರೆ {
		Tcl_ಗಣ_Obj_ಫಲಿತಾಂಶ(interp, Tcl_NewStringObj(ಮೌಲ್ಯ.as_string().c_str(), -1));
	}
	ಹಿಂತಿರುಗಿ TCL_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tcl_ಗಣ_param(Client_ದತ್ತಾಂಶ, Tcl_Interp *interp, ಪೂರ್ಣಾಂಕ objc, Tcl_Obj *ಸ್ಥಿರಾಂಕ objv[])
{
	ಪೂರ್ಣಾಂಕ i;
	ತಾರ್ಕಿಕ string_flag = ಸುಳ್ಳು, sint_flag = ಸುಳ್ಳು, uint_flag = ಸುಳ್ಳು;
	ಗಾಗಿ (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	ಸ್ವಯಂಚಾಲಿತ mod_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ಆದರೆ (!mod)
		ERROR("module not found")

	ಸ್ವಯಂಚಾಲಿತ cell_twine = yosys_design->twines().ಕಂಡುಹಿಡಿ(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	ಆದರೆ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().ಕೂಡಿಸು(param_id);

	ಆದರೆ (string_flag) {
		cell->ಗಣ_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ಮೌಲ್ಯ_mp;

		ಆದರೆ (Tcl_TakeBignumFromObj(interp, objv[i++], &ಮೌಲ್ಯ_mp))
			ERROR("non-integral value")

		ಆದರೆ (!mp_int_ಗಮ್ಯ_const(&ಮೌಲ್ಯ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ಆದರೆ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ಆದರೆ (const_.ಗಾತ್ರ_2() < 32)
				const_.exts(32);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (const_.ಗಾತ್ರ_2() < 32)
				const_.extu(32);
		}

		cell->ಗಣ_Param(param_twine, const_);
	} ಇಲ್ಲದಿದ್ದರೆ {
		cell->ಗಣ_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ಹಿಂತಿರುಗಿ TCL_OK;
}

ಪೂರ್ಣಾಂಕ yosys_tcl_interp_init(Tcl_Interp *interp)
{
	ಆದರೆ (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_ಸಂದೇಶ(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ಖಾಲಿ, ಖಾಲಿ);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ಖಾಲಿ, ಖಾಲಿ);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ಖಾಲಿ, ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_ಗಣ_attr, ಖಾಲಿ, ಖಾಲಿ);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ಖಾಲಿ, ಖಾಲಿ);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_ಗಣ_param, ಖಾಲಿ, ಖಾಲಿ);

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

	ಹಿಂತಿರುಗಿ TCL_OK ;
}
#ಆದರೆ_ಅಂತ್ಯ

YOSYS_NAMESPACE_ಕೊನೆ
