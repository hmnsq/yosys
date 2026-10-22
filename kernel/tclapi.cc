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

#شامل "kernel/yosys.h"
#شامل "kernel/rtlil.h"
#شامل "libs/json11/json11.hpp"

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
#شامل <tcl.h>
#شامل <tclTomMath.h>
#شامل <tclTomMathDecls.h>
#اگر_ختم

YOSYS_NAMESPACE_BEGIN

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
منطقی yosys_tcl_repl_فعال = جھوٹ;

خالی yosys_tcl_activate_repl()
{
	yosys_tcl_repl_فعال = سچ;
}

ساکن Tcl_Obj *json_مقصد_tcl(Tcl_Interp *interp, ثابت json11::Json &json)
{
	اگر (json.is_null())
		واپس Tcl_NewStringObj("null", 4);
	ورنہ اگر (json.is_string()) {
		خودکار string = json.string_قدر();
		واپس Tcl_NewStringObj(string.ڈیٹا(), string.سائز());
	} ورنہ اگر (json.is_نمبر()) {
		دوہرا قدر = json.نمبر_قدر();
		دوہرا باری_val = std::nearbyint(قدر);
		اگر (std::isfinite(باری_val) && قدر == باری_val && قدر >= LONG_MIN && قدر < -دوہرا(LONG_MIN))
			واپس Tcl_NewLongObj((طویل)باری_val);
		ورنہ
			واپس Tcl_NewDoubleObj(قدر);
	} ورنہ اگر (json.is_bool()) {
		واپس Tcl_NewBooleanObj(json.bool_قدر());
	} ورنہ اگر (json.is_array()) {
		خودکار list = json.array_اشیاء();
		Tcl_Obj *نتیجہ = Tcl_NewListObj(list.سائز(), خالی_اشاریہ);
		برائے (خودکار &شے_2 : list)
			Tcl_ListObjAppendElement(interp, نتیجہ, json_مقصد_tcl(interp, شے_2));
		واپس نتیجہ;
	} ورنہ اگر (json.is_object()) {
		خودکار map = json.object_اشیاء();
		Tcl_Obj *نتیجہ = Tcl_NewListObj(map.سائز() * 2, خالی_اشاریہ);
		برائے (خودکار &شے_2 : map) {
			Tcl_ListObjAppendElement(interp, نتیجہ, Tcl_NewStringObj(شے_2.پہلا.ڈیٹا(), شے_2.پہلا.سائز()));
			Tcl_ListObjAppendElement(interp, نتیجہ, json_مقصد_tcl(interp, شے_2.دوسرا));
		}
		واپس نتیجہ;
	} ورنہ {
		log_abort();
	}
}

ساکن صحیح tcl_yosys_cmd(Client_ڈیٹا, Tcl_Interp *interp, صحیح argc, ثابت حرف *argv[])
{
	std::سمتیہ<std::string> دلائل_2;
	برائے (صحیح i = 1; i < argc; i++)
		دلائل_2.ڈالو_عقب(argv[i]);

	اگر (دلائل_2.سائز() >= 1 && دلائل_2[0] == "-import") {
		برائے (خودکار &it : pass_رجسٹر) {
			std::string tcl_command_name = it.پہلا;
			اگر (tcl_command_name == "proc")
				tcl_command_name = "procs";
			ورنہ اگر (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			اگر (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				لوگارتھم("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.پہلا);
			} ورنہ {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.پہلا);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		واپس TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	منطقی اندر_repl = yosys_tcl_repl_فعال;
	خودکار guard = logger().error_throw_scope();
	کوشش {
		اگر (دلائل_2.سائز() == 1) {
			Pass::call(yosys_get_design(), دلائل_2[0]);
		} ورنہ {
			Pass::call(yosys_get_design(), دلائل_2);
		}
	} پکڑو (log_cmd_error_exception) {
		اگر (اندر_repl) {
			خودکار design = yosys_get_design();
			جب (design->selection_ڈھیر.سائز() > 1)
				design->نکالو_selection();
			log_reset_ڈھیر();
		}
		Tcl_سیٹ_نتیجہ(interp, (حرف *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_فعال = اندر_repl;
		واپس TCL_ERROR;
	} پکڑو (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_فعال = اندر_repl;

	خودکار &scratchpad = yosys_get_design()->scratchpad;
	خودکار نتیجہ = scratchpad.ڈھونڈو("result.json");
	اگر (نتیجہ != scratchpad.انتہا()) {
		std::string err;
		خودکار json = json11::Json::parse(نتیجہ->دوسرا, err);
		اگر (err.خالی_خانے()) {
			Tcl_سیٹ_Obj_نتیجہ(interp, json_مقصد_tcl(interp, json));
		} ورنہ
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} ورنہ اگر ((نتیجہ = scratchpad.ڈھونڈو("result.string")) != scratchpad.انتہا()) {
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewStringObj(نتیجہ->دوسرا.ڈیٹا(), نتیجہ->دوسرا.سائز()));
	}

	واپس TCL_OK;
}

#تعریف_کرو FLAG(name) \
	اگر (!strcmp(argv[i], "-" #name)) { \
		name##_flag = سچ; \
		جاری; \
	} \

#تعریف_کرو FLAG2(name) \
	اگر (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = سچ; \
		جاری; \
	} \

#تعریف_کرو ERROR(str) \
	{ \
		Tcl_سیٹ_نتیجہ(interp, (حرف *)(str), TCL_STATIC); \
		واپس TCL_ERROR; \
	}

منطقی const_مقصد_mp_int(ثابت Const &a, mp_int *b, منطقی force_signed, منطقی force_unsigned)
{
	اگر (!a.is_fully_def())
		واپس جھوٹ;

	اگر (mp_init(b))
		واپس جھوٹ;

	منطقی negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.خالی_خانے() && (a.عقب() == RTLIL::S1);

	برائے (صحیح i = a.سائز() - 1; i >= 0; i--) {
		اگر (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			واپس جھوٹ;
		}

		اگر ((a[i] == RTLIL::S1) ^ negative) {
			اگر (mp_جوڑو_d(b, 1, b)) {
				mp_clear(b);
				واپس جھوٹ;
			}
		}
	}

	اگر (negative) {
		اگر (mp_جوڑو_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			واپس جھوٹ;
		}
	}

	واپس سچ;
}

منطقی mp_int_مقصد_const(mp_int *a, Const &b, منطقی is_signed)
{
	منطقی negative = (mp_cmp_d(a, 0) == MP_LT);
	اگر (negative && !is_signed)
		واپس جھوٹ;

	اگر (negative) {
		اگر (mp_neg(a, a) != MP_OKAY)
			واپس جھوٹ;
		اگر (mp_sub_d(a, 1, a) != MP_OKAY)
			واپس جھوٹ;
	}

	std::سمتیہ<غیرعلامتی حرف> buf;
	buf.resize(mp_unsigned_bin_سائز(a));
	اگر (mp_مقصد_unsigned_bin(a, buf.ڈیٹا()) != MP_OKAY)
		واپس جھوٹ;

	Const::Builder b_بٹ(mp_گنتی_بٹ(a) + is_signed);
	برائے (صحیح i = 0; i < mp_گنتی_بٹ(a);) {
		برائے (صحیح j = 0; j < 8 && i < mp_گنتی_بٹ(a); j++, i++) {
			منطقی bv = ((buf.عقب() & (1 << j)) != 0) ^ negative;
			b_بٹ.ڈالو_عقب(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.نکالو_عقب();
	}

	اگر (is_signed) {
		b_بٹ.ڈالو_عقب(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_بٹ.build();

	واپس سچ;
}

ساکن صحیح tcl_get_attr(Client_ڈیٹا, Tcl_Interp *interp, صحیح argc, ثابت حرف *argv[])
{
	صحیح i;
	منطقی mod_flag = جھوٹ, string_flag = جھوٹ, bool_flag = جھوٹ;
	منطقی int_flag = جھوٹ, sint_flag = جھوٹ, uint_flag = جھوٹ;
	برائے (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(صحیح)
		FLAG(sint)
		FLAG(uint)
		FLAG(منطقی)
		توڑو;
	}

	اگر ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	اگر (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	خودکار mod_twine = yosys_design->twines().ڈھونڈو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = خالی_اشاریہ;
	اگر (mod_flag) {
		obj = mod;
	} ورنہ {
		خودکار obj_twine = yosys_design->twines().ڈھونڈو(obj_id);
		obj = mod->wire(obj_twine);
		اگر (!obj)
			obj = mod->memories.at(obj_twine, خالی_اشاریہ);
		اگر (!obj)
			obj = mod->cell(obj_twine);
		اگر (!obj)
			obj = mod->processes.at(obj_twine, خالی_اشاریہ);
	}

	اگر (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().ڈھونڈو(attr_id);

	اگر (string_flag) {
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} ورنہ اگر (int_flag || uint_flag || sint_flag) {
		اگر (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &قدر = obj->اوصاف.at(attr_twine);

		mp_int قدر_mp;
		اگر (!const_مقصد_mp_int(قدر, &قدر_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewBignumObj(&قدر_mp));
	} ورنہ اگر (bool_flag) {
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} ورنہ {
		اگر (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewStringObj(obj->اوصاف.at(attr_twine).as_string().c_str(), -1));
	}

	واپس TCL_OK;
}

ساکن صحیح tcl_has_attr(Client_ڈیٹا, Tcl_Interp *interp, صحیح argc, ثابت حرف *argv[])
{
	صحیح i;
	منطقی mod_flag = جھوٹ;
	برائے (i = 1; i < argc; i++) {
		FLAG(mod)
		توڑو;
	}

	اگر ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	اگر (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	خودکار mod_twine = yosys_design->twines().ڈھونڈو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = خالی_اشاریہ;
	اگر (mod_flag) {
		obj = mod;
	} ورنہ {
		خودکار obj_twine = yosys_design->twines().ڈھونڈو(obj_id);
		obj = mod->wire(obj_twine);
		اگر (!obj)
			obj = mod->memories.at(obj_twine, خالی_اشاریہ);
		اگر (!obj)
			obj = mod->cell(obj_twine);
		اگر (!obj)
			obj = mod->processes.at(obj_twine, خالی_اشاریہ);
	}

	اگر (!obj)
		ERROR("object not found")

	Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewStringObj(std::مقصد_string(obj->has_attribute(yosys_design->twines().ڈھونڈو(attr_id))).c_str(), -1));
	واپس TCL_OK;
}

ساکن صحیح tcl_سیٹ_attr(Client_ڈیٹا, Tcl_Interp *interp, صحیح objc, Tcl_Obj *ثابت objv[])
{
	صحیح i;
	منطقی mod_flag = جھوٹ, string_flag = جھوٹ, bool_flag = جھوٹ;
	منطقی true_flag = جھوٹ, false_flag = جھوٹ, sint_flag = جھوٹ, uint_flag = جھوٹ;
	برائے (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(سچ)
		FLAG2(جھوٹ)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(منطقی)
		توڑو;
	}

	اگر ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	اگر (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	خودکار mod_twine = yosys_design->twines().ڈھونڈو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = خالی_اشاریہ;
	اگر (mod_flag) {
		obj = mod;
	} ورنہ {
		خودکار obj_twine = yosys_design->twines().ڈھونڈو(obj_id);
		obj = mod->wire(obj_twine);
		اگر (!obj)
			obj = mod->memories.at(obj_twine, خالی_اشاریہ);
		اگر (!obj)
			obj = mod->cell(obj_twine);
		اگر (!obj)
			obj = mod->processes.at(obj_twine, خالی_اشاریہ);
	}

	اگر (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().جوڑو(attr_id);

	اگر (string_flag) {
		obj->سیٹ_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} ورنہ اگر (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int قدر_mp;

		اگر (Tcl_TakeBignumFromObj(interp, objv[i++], &قدر_mp))
			ERROR("non-integral value")

		اگر (!mp_int_مقصد_const(&قدر_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		اگر (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			اگر (const_.سائز() < 32)
				const_.exts(32);
		} ورنہ {
			اگر (const_.سائز() < 32)
				const_.extu(32);
		}

		obj->اوصاف[attr_twine] = const_;
	} ورنہ اگر (bool_flag) {
		obj->سیٹ_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} ورنہ اگر (true_flag) {
		obj->سیٹ_bool_attribute(attr_twine, سچ);
	} ورنہ اگر (false_flag) {
		obj->سیٹ_bool_attribute(attr_twine, جھوٹ);
	} ورنہ {
		obj->اوصاف[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	واپس TCL_OK;
}

ساکن صحیح tcl_get_param(Client_ڈیٹا, Tcl_Interp *interp, صحیح argc, ثابت حرف *argv[])
{
	صحیح i;
	منطقی string_flag = جھوٹ;
	منطقی int_flag = جھوٹ, sint_flag = جھوٹ, uint_flag = جھوٹ;
	برائے (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(صحیح)
		FLAG(sint)
		FLAG(uint)
		توڑو;
	}

	اگر ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	خودکار mod_twine = yosys_design->twines().ڈھونڈو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	خودکار cell_twine = yosys_design->twines().ڈھونڈو(cell_id);
	Cell* cell = mod->cell(cell_twine);
	اگر (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().ڈھونڈو(param_id);
	اگر (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ثابت RTLIL::Const &قدر = cell->getParam(param_twine);

	اگر (string_flag) {
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewStringObj(قدر.decode_string().c_str(), -1));
	} ورنہ اگر (int_flag || uint_flag || sint_flag) {
		mp_int قدر_mp;
		اگر (!const_مقصد_mp_int(قدر, &قدر_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewBignumObj(&قدر_mp));
	} ورنہ {
		Tcl_سیٹ_Obj_نتیجہ(interp, Tcl_NewStringObj(قدر.as_string().c_str(), -1));
	}
	واپس TCL_OK;
}

ساکن صحیح tcl_سیٹ_param(Client_ڈیٹا, Tcl_Interp *interp, صحیح objc, Tcl_Obj *ثابت objv[])
{
	صحیح i;
	منطقی string_flag = جھوٹ, sint_flag = جھوٹ, uint_flag = جھوٹ;
	برائے (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		توڑو;
	}

	اگر ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	خودکار mod_twine = yosys_design->twines().ڈھونڈو(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	خودکار cell_twine = yosys_design->twines().ڈھونڈو(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	اگر (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().جوڑو(param_id);

	اگر (string_flag) {
		cell->سیٹ_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} ورنہ اگر (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int قدر_mp;

		اگر (Tcl_TakeBignumFromObj(interp, objv[i++], &قدر_mp))
			ERROR("non-integral value")

		اگر (!mp_int_مقصد_const(&قدر_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		اگر (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			اگر (const_.سائز() < 32)
				const_.exts(32);
		} ورنہ {
			اگر (const_.سائز() < 32)
				const_.extu(32);
		}

		cell->سیٹ_Param(param_twine, const_);
	} ورنہ {
		cell->سیٹ_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	واپس TCL_OK;
}

صحیح yosys_tcl_interp_init(Tcl_Interp *interp)
{
	اگر (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_پیغام(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, تہی, تہی);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, تہی, تہی);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, تہی, تہی);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_سیٹ_attr, تہی, تہی);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, تہی, تہی);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_سیٹ_param, تہی, تہی);

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

	واپس TCL_OK ;
}
#اگر_ختم

YOSYS_NAMESPACE_انتہا
