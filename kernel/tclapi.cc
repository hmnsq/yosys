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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/json11/json11.hpp"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
#ਅਨ੍ਤਰ੍ਭਾਵ <tcl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <tclTomMath.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <tclTomMathDecls.h>
#ਜੇ_ਅੰਤ

YOSYS_NAMESPACE_BEGIN

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
ਤਾਰਕਿਕ yosys_tcl_repl_ਸਰਗਰਮ = ਝੂਠ;

ਖਾਲੀ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_ਸਰਗਰਮ = ਸੱਚ;
}

ਸਥਿਰ Tcl_Obj *json_ਮੰਜ਼ਿਲ_tcl(Tcl_Interp *interp, ਅਚਲ json11::Json &json)
{
	ਜੇ (json.is_null())
		ਵਾਪਸ Tcl_NewStringObj("null", 4);
	ਨਹੀਂਤਾਂ ਜੇ (json.is_string()) {
		ਸਵੈਚਾਲਿਤ string = json.string_ਮੁੱਲ();
		ਵਾਪਸ Tcl_NewStringObj(string.ਅੰਕੜੇ(), string.ਮਾਪ());
	} ਨਹੀਂਤਾਂ ਜੇ (json.is_ਸੰਖਿਆ()) {
		ਦੁੱਗਣਾ ਮੁੱਲ = json.ਸੰਖਿਆ_ਮੁੱਲ();
		ਦੁੱਗਣਾ ਗੇੜਾ_val = std::nearbyint(ਮੁੱਲ);
		ਜੇ (std::isfinite(ਗੇੜਾ_val) && ਮੁੱਲ == ਗੇੜਾ_val && ਮੁੱਲ >= LONG_MIN && ਮੁੱਲ < -ਦੁੱਗਣਾ(LONG_MIN))
			ਵਾਪਸ Tcl_NewLongObj((ਲੰਮਾ)ਗੇੜਾ_val);
		ਨਹੀਂਤਾਂ
			ਵਾਪਸ Tcl_NewDoubleObj(ਮੁੱਲ);
	} ਨਹੀਂਤਾਂ ਜੇ (json.is_bool()) {
		ਵਾਪਸ Tcl_NewBooleanObj(json.bool_ਮੁੱਲ());
	} ਨਹੀਂਤਾਂ ਜੇ (json.is_array()) {
		ਸਵੈਚਾਲਿਤ list = json.array_ਚੀਜ਼ਾਂ();
		Tcl_Obj *ਨਤੀਜਾ = Tcl_NewListObj(list.ਮਾਪ(), ਖਾਲੀ_ਸੂਚਕ);
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਵਸਤੂ_2 : list)
			Tcl_ListObjAppendElement(interp, ਨਤੀਜਾ, json_ਮੰਜ਼ਿਲ_tcl(interp, ਵਸਤੂ_2));
		ਵਾਪਸ ਨਤੀਜਾ;
	} ਨਹੀਂਤਾਂ ਜੇ (json.is_object()) {
		ਸਵੈਚਾਲਿਤ map = json.object_ਚੀਜ਼ਾਂ();
		Tcl_Obj *ਨਤੀਜਾ = Tcl_NewListObj(map.ਮਾਪ() * 2, ਖਾਲੀ_ਸੂਚਕ);
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਵਸਤੂ_2 : map) {
			Tcl_ListObjAppendElement(interp, ਨਤੀਜਾ, Tcl_NewStringObj(ਵਸਤੂ_2.ਪਹਿਲਾ.ਅੰਕੜੇ(), ਵਸਤੂ_2.ਪਹਿਲਾ.ਮਾਪ()));
			Tcl_ListObjAppendElement(interp, ਨਤੀਜਾ, json_ਮੰਜ਼ਿਲ_tcl(interp, ਵਸਤੂ_2.ਦੂਜਾ));
		}
		ਵਾਪਸ ਨਤੀਜਾ;
	} ਨਹੀਂਤਾਂ {
		log_abort();
	}
}

ਸਥਿਰ ਪੂਰਨ tcl_yosys_cmd(Client_ਅੰਕੜੇ, Tcl_Interp *interp, ਪੂਰਨ argc, ਅਚਲ ਅੱਖਰ *argv[])
{
	std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2;
	ਲਈ (ਪੂਰਨ i = 1; i < argc; i++)
		ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(argv[i]);

	ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() >= 1 && ਦਲੀਲਾਂ_2[0] == "-import") {
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : pass_ਰਜਿਸਟਰ) {
			std::string tcl_command_name = it.ਪਹਿਲਾ;
			ਜੇ (tcl_command_name == "proc")
				tcl_command_name = "procs";
			ਨਹੀਂਤਾਂ ਜੇ (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			ਜੇ (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				ਲਘੂਗਣਕ("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.ਪਹਿਲਾ);
			} ਨਹੀਂਤਾਂ {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.ਪਹਿਲਾ);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ਵਾਪਸ TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	ਤਾਰਕਿਕ ਵਿੱਚ_repl = yosys_tcl_repl_ਸਰਗਰਮ;
	ਸਵੈਚਾਲਿਤ guard = logger().error_throw_scope();
	ਕੋਸ਼ਿਸ਼ {
		ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 1) {
			Pass::call(yosys_get_design(), ਦਲੀਲਾਂ_2[0]);
		} ਨਹੀਂਤਾਂ {
			Pass::call(yosys_get_design(), ਦਲੀਲਾਂ_2);
		}
	} ਫੜੋ (log_cmd_error_exception) {
		ਜੇ (ਵਿੱਚ_repl) {
			ਸਵੈਚਾਲਿਤ design = yosys_get_design();
			ਜਦੋਂ (design->selection_ਢੇਰ.ਮਾਪ() > 1)
				design->ਕੱਢੋ_selection();
			log_reset_ਢੇਰ();
		}
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, (ਅੱਖਰ *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_ਸਰਗਰਮ = ਵਿੱਚ_repl;
		ਵਾਪਸ TCL_ERROR;
	} ਫੜੋ (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_ਸਰਗਰਮ = ਵਿੱਚ_repl;

	ਸਵੈਚਾਲਿਤ &scratchpad = yosys_get_design()->scratchpad;
	ਸਵੈਚਾਲਿਤ ਨਤੀਜਾ = scratchpad.ਲੱਭੋ("result.json");
	ਜੇ (ਨਤੀਜਾ != scratchpad.ਸਿਰਾ()) {
		std::string err;
		ਸਵੈਚਾਲਿਤ json = json11::Json::parse(ਨਤੀਜਾ->ਦੂਜਾ, err);
		ਜੇ (err.ਖਾਲੀ_ਥਾਵਾਂ()) {
			Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, json_ਮੰਜ਼ਿਲ_tcl(interp, json));
		} ਨਹੀਂਤਾਂ
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} ਨਹੀਂਤਾਂ ਜੇ ((ਨਤੀਜਾ = scratchpad.ਲੱਭੋ("result.string")) != scratchpad.ਸਿਰਾ()) {
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewStringObj(ਨਤੀਜਾ->ਦੂਜਾ.ਅੰਕੜੇ(), ਨਤੀਜਾ->ਦੂਜਾ.ਮਾਪ()));
	}

	ਵਾਪਸ TCL_OK;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLAG(name) \
	ਜੇ (!strcmp(argv[i], "-" #name)) { \
		name##_flag = ਸੱਚ; \
		ਜਾਰੀ; \
	} \

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLAG2(name) \
	ਜੇ (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = ਸੱਚ; \
		ਜਾਰੀ; \
	} \

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERROR(str) \
	{ \
		Tcl_ਸੈੱਟ_ਨਤੀਜਾ(interp, (ਅੱਖਰ *)(str), TCL_STATIC); \
		ਵਾਪਸ TCL_ERROR; \
	}

ਤਾਰਕਿਕ const_ਮੰਜ਼ਿਲ_mp_int(ਅਚਲ Const &a, mp_int *b, ਤਾਰਕਿਕ force_signed, ਤਾਰਕਿਕ force_unsigned)
{
	ਜੇ (!a.is_fully_def())
		ਵਾਪਸ ਝੂਠ;

	ਜੇ (mp_init(b))
		ਵਾਪਸ ਝੂਠ;

	ਤਾਰਕਿਕ negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.ਖਾਲੀ_ਥਾਵਾਂ() && (a.ਪਿੱਠ() == RTLIL::S1);

	ਲਈ (ਪੂਰਨ i = a.ਮਾਪ() - 1; i >= 0; i--) {
		ਜੇ (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ਵਾਪਸ ਝੂਠ;
		}

		ਜੇ ((a[i] == RTLIL::S1) ^ negative) {
			ਜੇ (mp_ਜੋੜੋ_d(b, 1, b)) {
				mp_clear(b);
				ਵਾਪਸ ਝੂਠ;
			}
		}
	}

	ਜੇ (negative) {
		ਜੇ (mp_ਜੋੜੋ_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ਵਾਪਸ ਝੂਠ;
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ mp_int_ਮੰਜ਼ਿਲ_const(mp_int *a, Const &b, ਤਾਰਕਿਕ is_signed)
{
	ਤਾਰਕਿਕ negative = (mp_cmp_d(a, 0) == MP_LT);
	ਜੇ (negative && !is_signed)
		ਵਾਪਸ ਝੂਠ;

	ਜੇ (negative) {
		ਜੇ (mp_neg(a, a) != MP_OKAY)
			ਵਾਪਸ ਝੂਠ;
		ਜੇ (mp_sub_d(a, 1, a) != MP_OKAY)
			ਵਾਪਸ ਝੂਠ;
	}

	std::ਸਦਿਸ਼<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ> buf;
	buf.resize(mp_unsigned_bin_ਮਾਪ(a));
	ਜੇ (mp_ਮੰਜ਼ਿਲ_unsigned_bin(a, buf.ਅੰਕੜੇ()) != MP_OKAY)
		ਵਾਪਸ ਝੂਠ;

	Const::Builder b_ਬਿੱਟ(mp_ਗਿਣਤੀ_ਬਿੱਟ(a) + is_signed);
	ਲਈ (ਪੂਰਨ i = 0; i < mp_ਗਿਣਤੀ_ਬਿੱਟ(a);) {
		ਲਈ (ਪੂਰਨ j = 0; j < 8 && i < mp_ਗਿਣਤੀ_ਬਿੱਟ(a); j++, i++) {
			ਤਾਰਕਿਕ bv = ((buf.ਪਿੱਠ() & (1 << j)) != 0) ^ negative;
			b_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.ਕੱਢੋ_ਪਿੱਠ();
	}

	ਜੇ (is_signed) {
		b_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_ਬਿੱਟ.build();

	ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਪੂਰਨ tcl_get_attr(Client_ਅੰਕੜੇ, Tcl_Interp *interp, ਪੂਰਨ argc, ਅਚਲ ਅੱਖਰ *argv[])
{
	ਪੂਰਨ i;
	ਤਾਰਕਿਕ mod_flag = ਝੂਠ, string_flag = ਝੂਠ, bool_flag = ਝੂਠ;
	ਤਾਰਕਿਕ int_flag = ਝੂਠ, sint_flag = ਝੂਠ, uint_flag = ਝੂਠ;
	ਲਈ (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(ਪੂਰਨ)
		FLAG(sint)
		FLAG(uint)
		FLAG(ਤਾਰਕਿਕ)
		ਰੋਕੋ;
	}

	ਜੇ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ਜੇ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	ਸਵੈਚਾਲਿਤ mod_twine = yosys_design->twines().ਲੱਭੋ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ਜੇ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = ਖਾਲੀ_ਸੂਚਕ;
	ਜੇ (mod_flag) {
		obj = mod;
	} ਨਹੀਂਤਾਂ {
		ਸਵੈਚਾਲਿਤ obj_twine = yosys_design->twines().ਲੱਭੋ(obj_id);
		obj = mod->wire(obj_twine);
		ਜੇ (!obj)
			obj = mod->memories.at(obj_twine, ਖਾਲੀ_ਸੂਚਕ);
		ਜੇ (!obj)
			obj = mod->cell(obj_twine);
		ਜੇ (!obj)
			obj = mod->processes.at(obj_twine, ਖਾਲੀ_ਸੂਚਕ);
	}

	ਜੇ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().ਲੱਭੋ(attr_id);

	ਜੇ (string_flag) {
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} ਨਹੀਂਤਾਂ ਜੇ (int_flag || uint_flag || sint_flag) {
		ਜੇ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &ਮੁੱਲ = obj->ਗੁਣ.at(attr_twine);

		mp_int ਮੁੱਲ_mp;
		ਜੇ (!const_ਮੰਜ਼ਿਲ_mp_int(ਮੁੱਲ, &ਮੁੱਲ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewBignumObj(&ਮੁੱਲ_mp));
	} ਨਹੀਂਤਾਂ ਜੇ (bool_flag) {
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewStringObj(obj->ਗੁਣ.at(attr_twine).as_string().c_str(), -1));
	}

	ਵਾਪਸ TCL_OK;
}

ਸਥਿਰ ਪੂਰਨ tcl_has_attr(Client_ਅੰਕੜੇ, Tcl_Interp *interp, ਪੂਰਨ argc, ਅਚਲ ਅੱਖਰ *argv[])
{
	ਪੂਰਨ i;
	ਤਾਰਕਿਕ mod_flag = ਝੂਠ;
	ਲਈ (i = 1; i < argc; i++) {
		FLAG(mod)
		ਰੋਕੋ;
	}

	ਜੇ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ਜੇ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	ਸਵੈਚਾਲਿਤ mod_twine = yosys_design->twines().ਲੱਭੋ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ਜੇ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = ਖਾਲੀ_ਸੂਚਕ;
	ਜੇ (mod_flag) {
		obj = mod;
	} ਨਹੀਂਤਾਂ {
		ਸਵੈਚਾਲਿਤ obj_twine = yosys_design->twines().ਲੱਭੋ(obj_id);
		obj = mod->wire(obj_twine);
		ਜੇ (!obj)
			obj = mod->memories.at(obj_twine, ਖਾਲੀ_ਸੂਚਕ);
		ਜੇ (!obj)
			obj = mod->cell(obj_twine);
		ਜੇ (!obj)
			obj = mod->processes.at(obj_twine, ਖਾਲੀ_ਸੂਚਕ);
	}

	ਜੇ (!obj)
		ERROR("object not found")

	Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewStringObj(std::ਮੰਜ਼ਿਲ_string(obj->has_attribute(yosys_design->twines().ਲੱਭੋ(attr_id))).c_str(), -1));
	ਵਾਪਸ TCL_OK;
}

ਸਥਿਰ ਪੂਰਨ tcl_ਸੈੱਟ_attr(Client_ਅੰਕੜੇ, Tcl_Interp *interp, ਪੂਰਨ objc, Tcl_Obj *ਅਚਲ objv[])
{
	ਪੂਰਨ i;
	ਤਾਰਕਿਕ mod_flag = ਝੂਠ, string_flag = ਝੂਠ, bool_flag = ਝੂਠ;
	ਤਾਰਕਿਕ true_flag = ਝੂਠ, false_flag = ਝੂਠ, sint_flag = ਝੂਠ, uint_flag = ਝੂਠ;
	ਲਈ (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(ਸੱਚ)
		FLAG2(ਝੂਠ)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(ਤਾਰਕਿਕ)
		ਰੋਕੋ;
	}

	ਜੇ ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	ਜੇ (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	ਸਵੈਚਾਲਿਤ mod_twine = yosys_design->twines().ਲੱਭੋ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ਜੇ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = ਖਾਲੀ_ਸੂਚਕ;
	ਜੇ (mod_flag) {
		obj = mod;
	} ਨਹੀਂਤਾਂ {
		ਸਵੈਚਾਲਿਤ obj_twine = yosys_design->twines().ਲੱਭੋ(obj_id);
		obj = mod->wire(obj_twine);
		ਜੇ (!obj)
			obj = mod->memories.at(obj_twine, ਖਾਲੀ_ਸੂਚਕ);
		ਜੇ (!obj)
			obj = mod->cell(obj_twine);
		ਜੇ (!obj)
			obj = mod->processes.at(obj_twine, ਖਾਲੀ_ਸੂਚਕ);
	}

	ਜੇ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().ਜੋੜੋ(attr_id);

	ਜੇ (string_flag) {
		obj->ਸੈੱਟ_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} ਨਹੀਂਤਾਂ ਜੇ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ਮੁੱਲ_mp;

		ਜੇ (Tcl_TakeBignumFromObj(interp, objv[i++], &ਮੁੱਲ_mp))
			ERROR("non-integral value")

		ਜੇ (!mp_int_ਮੰਜ਼ਿਲ_const(&ਮੁੱਲ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ਜੇ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ਜੇ (const_.ਮਾਪ() < 32)
				const_.exts(32);
		} ਨਹੀਂਤਾਂ {
			ਜੇ (const_.ਮਾਪ() < 32)
				const_.extu(32);
		}

		obj->ਗੁਣ[attr_twine] = const_;
	} ਨਹੀਂਤਾਂ ਜੇ (bool_flag) {
		obj->ਸੈੱਟ_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} ਨਹੀਂਤਾਂ ਜੇ (true_flag) {
		obj->ਸੈੱਟ_bool_attribute(attr_twine, ਸੱਚ);
	} ਨਹੀਂਤਾਂ ਜੇ (false_flag) {
		obj->ਸੈੱਟ_bool_attribute(attr_twine, ਝੂਠ);
	} ਨਹੀਂਤਾਂ {
		obj->ਗੁਣ[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ਵਾਪਸ TCL_OK;
}

ਸਥਿਰ ਪੂਰਨ tcl_get_param(Client_ਅੰਕੜੇ, Tcl_Interp *interp, ਪੂਰਨ argc, ਅਚਲ ਅੱਖਰ *argv[])
{
	ਪੂਰਨ i;
	ਤਾਰਕਿਕ string_flag = ਝੂਠ;
	ਤਾਰਕਿਕ int_flag = ਝੂਠ, sint_flag = ਝੂਠ, uint_flag = ਝੂਠ;
	ਲਈ (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(ਪੂਰਨ)
		FLAG(sint)
		FLAG(uint)
		ਰੋਕੋ;
	}

	ਜੇ ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	ਸਵੈਚਾਲਿਤ mod_twine = yosys_design->twines().ਲੱਭੋ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ਜੇ (!mod)
		ERROR("module not found")

	ਸਵੈਚਾਲਿਤ cell_twine = yosys_design->twines().ਲੱਭੋ(cell_id);
	Cell* cell = mod->cell(cell_twine);
	ਜੇ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().ਲੱਭੋ(param_id);
	ਜੇ (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ਅਚਲ RTLIL::Const &ਮੁੱਲ = cell->getParam(param_twine);

	ਜੇ (string_flag) {
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewStringObj(ਮੁੱਲ.decode_string().c_str(), -1));
	} ਨਹੀਂਤਾਂ ਜੇ (int_flag || uint_flag || sint_flag) {
		mp_int ਮੁੱਲ_mp;
		ਜੇ (!const_ਮੰਜ਼ਿਲ_mp_int(ਮੁੱਲ, &ਮੁੱਲ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewBignumObj(&ਮੁੱਲ_mp));
	} ਨਹੀਂਤਾਂ {
		Tcl_ਸੈੱਟ_Obj_ਨਤੀਜਾ(interp, Tcl_NewStringObj(ਮੁੱਲ.as_string().c_str(), -1));
	}
	ਵਾਪਸ TCL_OK;
}

ਸਥਿਰ ਪੂਰਨ tcl_ਸੈੱਟ_param(Client_ਅੰਕੜੇ, Tcl_Interp *interp, ਪੂਰਨ objc, Tcl_Obj *ਅਚਲ objv[])
{
	ਪੂਰਨ i;
	ਤਾਰਕਿਕ string_flag = ਝੂਠ, sint_flag = ਝੂਠ, uint_flag = ਝੂਠ;
	ਲਈ (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		ਰੋਕੋ;
	}

	ਜੇ ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	ਸਵੈਚਾਲਿਤ mod_twine = yosys_design->twines().ਲੱਭੋ(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ਜੇ (!mod)
		ERROR("module not found")

	ਸਵੈਚਾਲਿਤ cell_twine = yosys_design->twines().ਲੱਭੋ(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	ਜੇ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().ਜੋੜੋ(param_id);

	ਜੇ (string_flag) {
		cell->ਸੈੱਟ_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} ਨਹੀਂਤਾਂ ਜੇ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ਮੁੱਲ_mp;

		ਜੇ (Tcl_TakeBignumFromObj(interp, objv[i++], &ਮੁੱਲ_mp))
			ERROR("non-integral value")

		ਜੇ (!mp_int_ਮੰਜ਼ਿਲ_const(&ਮੁੱਲ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ਜੇ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ਜੇ (const_.ਮਾਪ() < 32)
				const_.exts(32);
		} ਨਹੀਂਤਾਂ {
			ਜੇ (const_.ਮਾਪ() < 32)
				const_.extu(32);
		}

		cell->ਸੈੱਟ_Param(param_twine, const_);
	} ਨਹੀਂਤਾਂ {
		cell->ਸੈੱਟ_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ਵਾਪਸ TCL_OK;
}

ਪੂਰਨ yosys_tcl_interp_init(Tcl_Interp *interp)
{
	ਜੇ (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_ਸੁਨੇਹਾ(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ਨਿਰਾ, ਨਿਰਾ);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ਨਿਰਾ, ਨਿਰਾ);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ਨਿਰਾ, ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_ਸੈੱਟ_attr, ਨਿਰਾ, ਨਿਰਾ);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ਨਿਰਾ, ਨਿਰਾ);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_ਸੈੱਟ_param, ਨਿਰਾ, ਨਿਰਾ);

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

	ਵਾਪਸ TCL_OK ;
}
#ਜੇ_ਅੰਤ

YOSYS_NAMESPACE_ਸਿਰਾ
