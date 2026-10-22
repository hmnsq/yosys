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

#ضمن "kernel/yosys.h"
#ضمن "kernel/rtlil.h"
#ضمن "libs/json11/json11.hpp"

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
#ضمن <tcl.h>
#ضمن <tclTomMath.h>
#ضمن <tclTomMathDecls.h>
#جيڪڏهن_پورو

YOSYS_NAMESPACE_BEGIN

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
منطقي yosys_tcl_repl_سرگرم = خطأ;

فراغ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_سرگرم = سچ;
}

ساكن Tcl_Obj *json_منزل_tcl(Tcl_Interp *interp, ثابت json11::Json &json)
{
	إذا (json.is_null())
		أرجع Tcl_NewStringObj("null", 4);
	وإلا إذا (json.is_string()) {
		خودڪار string = json.string_قدر();
		أرجع Tcl_NewStringObj(string.معلومات(), string.سائيز());
	} وإلا إذا (json.is_عدد()) {
		مزدوج قدر = json.عدد_قدر();
		مزدوج ڦيرو_val = std::nearbyint(قدر);
		إذا (std::isfinite(ڦيرو_val) && قدر == ڦيرو_val && قدر >= LONG_MIN && قدر < -مزدوج(LONG_MIN))
			أرجع Tcl_NewLongObj((ڊگهو)ڦيرو_val);
		وإلا
			أرجع Tcl_NewDoubleObj(قدر);
	} وإلا إذا (json.is_bool()) {
		أرجع Tcl_NewBooleanObj(json.bool_قدر());
	} وإلا إذا (json.is_array()) {
		خودڪار list = json.array_شيون();
		Tcl_Obj *نتيجو = Tcl_NewListObj(list.سائيز(), خالي_اشارو);
		لكل (خودڪار &شي : list)
			Tcl_ListObjAppendElement(interp, نتيجو, json_منزل_tcl(interp, شي));
		أرجع نتيجو;
	} وإلا إذا (json.is_object()) {
		خودڪار map = json.object_شيون();
		Tcl_Obj *نتيجو = Tcl_NewListObj(map.سائيز() * 2, خالي_اشارو);
		لكل (خودڪار &شي : map) {
			Tcl_ListObjAppendElement(interp, نتيجو, Tcl_NewStringObj(شي.پهريون.معلومات(), شي.پهريون.سائيز()));
			Tcl_ListObjAppendElement(interp, نتيجو, json_منزل_tcl(interp, شي.ٻيو));
		}
		أرجع نتيجو;
	} وإلا {
		log_abort();
	}
}

ساكن صحيح tcl_yosys_cmd(Client_معلومات, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	std::ويڪٽر<std::string> دليلون_2;
	لكل (صحيح i = 1; i < argc; i++)
		دليلون_2.ڌڪو_پٺ(argv[i]);

	إذا (دليلون_2.سائيز() >= 1 && دليلون_2[0] == "-import") {
		لكل (خودڪار &it : pass_رجسٽر) {
			std::string tcl_command_name = it.پهريون;
			إذا (tcl_command_name == "proc")
				tcl_command_name = "procs";
			وإلا إذا (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			إذا (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				لاگرٿم("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.پهريون);
			} وإلا {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.پهريون);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		أرجع TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	منطقي اندريون_repl = yosys_tcl_repl_سرگرم;
	خودڪار guard = logger().error_throw_scope();
	حاول {
		إذا (دليلون_2.سائيز() == 1) {
			Pass::call(yosys_get_design(), دليلون_2[0]);
		} وإلا {
			Pass::call(yosys_get_design(), دليلون_2);
		}
	} امسك (log_cmd_error_exception) {
		إذا (اندريون_repl) {
			خودڪار design = yosys_get_design();
			طالما (design->selection_ڍير.سائيز() > 1)
				design->ڪڍو_selection();
			log_reset_ڍير();
		}
		Tcl_سيٽ_نتيجو(interp, (حرف *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_سرگرم = اندريون_repl;
		أرجع TCL_ERROR;
	} امسك (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_سرگرم = اندريون_repl;

	خودڪار &scratchpad = yosys_get_design()->scratchpad;
	خودڪار نتيجو = scratchpad.ڳوليو("result.json");
	إذا (نتيجو != scratchpad.پڇاڙي()) {
		std::string err;
		خودڪار json = json11::Json::parse(نتيجو->ٻيو, err);
		إذا (err.خالي_جايون()) {
			Tcl_سيٽ_Obj_نتيجو(interp, json_منزل_tcl(interp, json));
		} وإلا
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} وإلا إذا ((نتيجو = scratchpad.ڳوليو("result.string")) != scratchpad.پڇاڙي()) {
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewStringObj(نتيجو->ٻيو.معلومات(), نتيجو->ٻيو.سائيز()));
	}

	أرجع TCL_OK;
}

#وصف_ڪريو FLAG(name) \
	إذا (!strcmp(argv[i], "-" #name)) { \
		name##_flag = سچ; \
		استمر; \
	} \

#وصف_ڪريو FLAG2(name) \
	إذا (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = سچ; \
		استمر; \
	} \

#وصف_ڪريو ERROR(str) \
	{ \
		Tcl_سيٽ_نتيجو(interp, (حرف *)(str), TCL_STATIC); \
		أرجع TCL_ERROR; \
	}

منطقي const_منزل_mp_int(ثابت Const &a, mp_int *b, منطقي force_signed, منطقي force_unsigned)
{
	إذا (!a.is_fully_def())
		أرجع خطأ;

	إذا (mp_init(b))
		أرجع خطأ;

	منطقي negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.خالي_جايون() && (a.پٺ() == RTLIL::S1);

	لكل (صحيح i = a.سائيز() - 1; i >= 0; i--) {
		إذا (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			أرجع خطأ;
		}

		إذا ((a[i] == RTLIL::S1) ^ negative) {
			إذا (mp_جوڙيو_d(b, 1, b)) {
				mp_clear(b);
				أرجع خطأ;
			}
		}
	}

	إذا (negative) {
		إذا (mp_جوڙيو_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			أرجع خطأ;
		}
	}

	أرجع سچ;
}

منطقي mp_int_منزل_const(mp_int *a, Const &b, منطقي is_signed)
{
	منطقي negative = (mp_cmp_d(a, 0) == MP_LT);
	إذا (negative && !is_signed)
		أرجع خطأ;

	إذا (negative) {
		إذا (mp_neg(a, a) != MP_OKAY)
			أرجع خطأ;
		إذا (mp_sub_d(a, 1, a) != MP_OKAY)
			أرجع خطأ;
	}

	std::ويڪٽر<بنانشان حرف> buf;
	buf.resize(mp_unsigned_bin_سائيز(a));
	إذا (mp_منزل_unsigned_bin(a, buf.معلومات()) != MP_OKAY)
		أرجع خطأ;

	Const::Builder b_بٽ(mp_ڳڻپ_بٽ(a) + is_signed);
	لكل (صحيح i = 0; i < mp_ڳڻپ_بٽ(a);) {
		لكل (صحيح j = 0; j < 8 && i < mp_ڳڻپ_بٽ(a); j++, i++) {
			منطقي bv = ((buf.پٺ() & (1 << j)) != 0) ^ negative;
			b_بٽ.ڌڪو_پٺ(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.ڪڍو_پٺ();
	}

	إذا (is_signed) {
		b_بٽ.ڌڪو_پٺ(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_بٽ.build();

	أرجع سچ;
}

ساكن صحيح tcl_get_attr(Client_معلومات, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	صحيح i;
	منطقي mod_flag = خطأ, string_flag = خطأ, bool_flag = خطأ;
	منطقي int_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(صحيح)
		FLAG(sint)
		FLAG(uint)
		FLAG(منطقي)
		اكسر;
	}

	إذا ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	إذا (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	خودڪار mod_twine = yosys_design->twines().ڳوليو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = خالي_اشارو;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خودڪار obj_twine = yosys_design->twines().ڳوليو(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, خالي_اشارو);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, خالي_اشارو);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().ڳوليو(attr_id);

	إذا (string_flag) {
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &قدر = obj->خاصيتون.at(attr_twine);

		mp_int قدر_mp;
		إذا (!const_منزل_mp_int(قدر, &قدر_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewBignumObj(&قدر_mp));
	} وإلا إذا (bool_flag) {
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} وإلا {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewStringObj(obj->خاصيتون.at(attr_twine).as_string().c_str(), -1));
	}

	أرجع TCL_OK;
}

ساكن صحيح tcl_has_attr(Client_معلومات, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	صحيح i;
	منطقي mod_flag = خطأ;
	لكل (i = 1; i < argc; i++) {
		FLAG(mod)
		اكسر;
	}

	إذا ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	إذا (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	خودڪار mod_twine = yosys_design->twines().ڳوليو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = خالي_اشارو;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خودڪار obj_twine = yosys_design->twines().ڳوليو(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, خالي_اشارو);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, خالي_اشارو);
	}

	إذا (!obj)
		ERROR("object not found")

	Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewStringObj(std::منزل_string(obj->has_attribute(yosys_design->twines().ڳوليو(attr_id))).c_str(), -1));
	أرجع TCL_OK;
}

ساكن صحيح tcl_سيٽ_attr(Client_معلومات, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
{
	صحيح i;
	منطقي mod_flag = خطأ, string_flag = خطأ, bool_flag = خطأ;
	منطقي true_flag = خطأ, false_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(سچ)
		FLAG2(خطأ)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(منطقي)
		اكسر;
	}

	إذا ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	إذا (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	خودڪار mod_twine = yosys_design->twines().ڳوليو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = خالي_اشارو;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خودڪار obj_twine = yosys_design->twines().ڳوليو(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, خالي_اشارو);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, خالي_اشارو);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().جوڙيو(attr_id);

	إذا (string_flag) {
		obj->سيٽ_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int قدر_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &قدر_mp))
			ERROR("non-integral value")

		إذا (!mp_int_منزل_const(&قدر_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.سائيز() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.سائيز() < 32)
				const_.extu(32);
		}

		obj->خاصيتون[attr_twine] = const_;
	} وإلا إذا (bool_flag) {
		obj->سيٽ_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} وإلا إذا (true_flag) {
		obj->سيٽ_bool_attribute(attr_twine, سچ);
	} وإلا إذا (false_flag) {
		obj->سيٽ_bool_attribute(attr_twine, خطأ);
	} وإلا {
		obj->خاصيتون[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	أرجع TCL_OK;
}

ساكن صحيح tcl_get_param(Client_معلومات, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	صحيح i;
	منطقي string_flag = خطأ;
	منطقي int_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(صحيح)
		FLAG(sint)
		FLAG(uint)
		اكسر;
	}

	إذا ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	خودڪار mod_twine = yosys_design->twines().ڳوليو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	خودڪار cell_twine = yosys_design->twines().ڳوليو(cell_id);
	Cell* cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().ڳوليو(param_id);
	إذا (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ثابت RTLIL::Const &قدر = cell->getParam(param_twine);

	إذا (string_flag) {
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewStringObj(قدر.decode_string().c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		mp_int قدر_mp;
		إذا (!const_منزل_mp_int(قدر, &قدر_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewBignumObj(&قدر_mp));
	} وإلا {
		Tcl_سيٽ_Obj_نتيجو(interp, Tcl_NewStringObj(قدر.as_string().c_str(), -1));
	}
	أرجع TCL_OK;
}

ساكن صحيح tcl_سيٽ_param(Client_معلومات, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
{
	صحيح i;
	منطقي string_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		اكسر;
	}

	إذا ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	خودڪار mod_twine = yosys_design->twines().ڳوليو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	خودڪار cell_twine = yosys_design->twines().ڳوليو(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().جوڙيو(param_id);

	إذا (string_flag) {
		cell->سيٽ_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int قدر_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &قدر_mp))
			ERROR("non-integral value")

		إذا (!mp_int_منزل_const(&قدر_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.سائيز() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.سائيز() < 32)
				const_.extu(32);
		}

		cell->سيٽ_Param(param_twine, const_);
	} وإلا {
		cell->سيٽ_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	أرجع TCL_OK;
}

صحيح yosys_tcl_interp_init(Tcl_Interp *interp)
{
	إذا (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_نياپو(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_سيٽ_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_سيٽ_param, عدم, عدم);

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

	أرجع TCL_OK ;
}
#جيڪڏهن_پورو

YOSYS_NAMESPACE_پڇاڙي
