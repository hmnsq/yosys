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

#که_تعریف_شوی YOSYS_ENABLE_TCL
#ضمن <tcl.h>
#ضمن <tclTomMath.h>
#ضمن <tclTomMathDecls.h>
#د_که_پای

YOSYS_NAMESPACE_BEGIN

#که_تعریف_شوی YOSYS_ENABLE_TCL
منطقي yosys_tcl_repl_فعال = خطأ;

فراغ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_فعال = ریښتیا;
}

ساكن Tcl_Obj *json_منزل_tcl(Tcl_Interp *interp, ثابت json11::Json &json)
{
	إذا (json.is_null())
		أرجع Tcl_NewStringObj("null", 4);
	وإلا إذا (json.is_string()) {
		خپلکاری string = json.string_ارزښت();
		أرجع Tcl_NewStringObj(string.معلومات(), string.اندازه());
	} وإلا إذا (json.is_شمېره()) {
		مزدوج ارزښت = json.شمېره_ارزښت();
		مزدوج پړاو_val = std::nearbyint(ارزښت);
		إذا (std::isfinite(پړاو_val) && ارزښت == پړاو_val && ارزښت >= LONG_MIN && ارزښت < -مزدوج(LONG_MIN))
			أرجع Tcl_NewLongObj((اوږد)پړاو_val);
		وإلا
			أرجع Tcl_NewDoubleObj(ارزښت);
	} وإلا إذا (json.is_bool()) {
		أرجع Tcl_NewBooleanObj(json.bool_ارزښت());
	} وإلا إذا (json.is_array()) {
		خپلکاری list = json.array_توکي();
		Tcl_Obj *پایله = Tcl_NewListObj(list.اندازه(), تش_اشاره);
		لكل (خپلکاری &توکی : list)
			Tcl_ListObjAppendElement(interp, پایله, json_منزل_tcl(interp, توکی));
		أرجع پایله;
	} وإلا إذا (json.is_object()) {
		خپلکاری map = json.object_توکي();
		Tcl_Obj *پایله = Tcl_NewListObj(map.اندازه() * 2, تش_اشاره);
		لكل (خپلکاری &توکی : map) {
			Tcl_ListObjAppendElement(interp, پایله, Tcl_NewStringObj(توکی.لومړی.معلومات(), توکی.لومړی.اندازه()));
			Tcl_ListObjAppendElement(interp, پایله, json_منزل_tcl(interp, توکی.دوهم));
		}
		أرجع پایله;
	} وإلا {
		log_abort();
	}
}

ساكن صحيح tcl_yosys_cmd(Client_معلومات, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	std::ویکټور<std::string> دلیلونه_2;
	لكل (صحيح i = 1; i < argc; i++)
		دلیلونه_2.ټېل_شا(argv[i]);

	إذا (دلیلونه_2.اندازه() >= 1 && دلیلونه_2[0] == "-import") {
		لكل (خپلکاری &it : pass_ثبتوونکی) {
			std::string tcl_command_name = it.لومړی;
			إذا (tcl_command_name == "proc")
				tcl_command_name = "procs";
			وإلا إذا (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			إذا (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				لوګاریتم("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.لومړی);
			} وإلا {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.لومړی);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		أرجع TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	منطقي دننه_repl = yosys_tcl_repl_فعال;
	خپلکاری guard = logger().error_throw_scope();
	حاول {
		إذا (دلیلونه_2.اندازه() == 1) {
			Pass::call(yosys_get_design(), دلیلونه_2[0]);
		} وإلا {
			Pass::call(yosys_get_design(), دلیلونه_2);
		}
	} امسك (log_cmd_error_exception) {
		إذا (دننه_repl) {
			خپلکاری design = yosys_get_design();
			طالما (design->selection_ډېری.اندازه() > 1)
				design->راوباسه_selection();
			log_reset_ډېری();
		}
		Tcl_سېټ_پایله(interp, (حرف *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_فعال = دننه_repl;
		أرجع TCL_ERROR;
	} امسك (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_فعال = دننه_repl;

	خپلکاری &scratchpad = yosys_get_design()->scratchpad;
	خپلکاری پایله = scratchpad.پیدا_کړه("result.json");
	إذا (پایله != scratchpad.پای_ټکی()) {
		std::string err;
		خپلکاری json = json11::Json::parse(پایله->دوهم, err);
		إذا (err.تشه()) {
			Tcl_سېټ_Obj_پایله(interp, json_منزل_tcl(interp, json));
		} وإلا
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} وإلا إذا ((پایله = scratchpad.پیدا_کړه("result.string")) != scratchpad.پای_ټکی()) {
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewStringObj(پایله->دوهم.معلومات(), پایله->دوهم.اندازه()));
	}

	أرجع TCL_OK;
}

#تعریف_کړه FLAG(name) \
	إذا (!strcmp(argv[i], "-" #name)) { \
		name##_flag = ریښتیا; \
		استمر; \
	} \

#تعریف_کړه FLAG2(name) \
	إذا (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = ریښتیا; \
		استمر; \
	} \

#تعریف_کړه ERROR(str) \
	{ \
		Tcl_سېټ_پایله(interp, (حرف *)(str), TCL_STATIC); \
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
					!a.تشه() && (a.شا() == RTLIL::S1);

	لكل (صحيح i = a.اندازه() - 1; i >= 0; i--) {
		إذا (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			أرجع خطأ;
		}

		إذا ((a[i] == RTLIL::S1) ^ negative) {
			إذا (mp_جمع_کړه_d(b, 1, b)) {
				mp_clear(b);
				أرجع خطأ;
			}
		}
	}

	إذا (negative) {
		إذا (mp_جمع_کړه_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			أرجع خطأ;
		}
	}

	أرجع ریښتیا;
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

	std::ویکټور<بینښه حرف> buf;
	buf.resize(mp_unsigned_bin_اندازه(a));
	إذا (mp_منزل_unsigned_bin(a, buf.معلومات()) != MP_OKAY)
		أرجع خطأ;

	Const::Builder b_بټونه(mp_شمېر_بټونه(a) + is_signed);
	لكل (صحيح i = 0; i < mp_شمېر_بټونه(a);) {
		لكل (صحيح j = 0; j < 8 && i < mp_شمېر_بټونه(a); j++, i++) {
			منطقي bv = ((buf.شا() & (1 << j)) != 0) ^ negative;
			b_بټونه.ټېل_شا(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.راوباسه_شا();
	}

	إذا (is_signed) {
		b_بټونه.ټېل_شا(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_بټونه.build();

	أرجع ریښتیا;
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

	خپلکاری mod_twine = yosys_design->twines().پیدا_کړه(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = تش_اشاره;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خپلکاری obj_twine = yosys_design->twines().پیدا_کړه(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, تش_اشاره);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, تش_اشاره);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().پیدا_کړه(attr_id);

	إذا (string_flag) {
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &ارزښت = obj->ځانګړتیاوې.at(attr_twine);

		mp_int ارزښت_mp;
		إذا (!const_منزل_mp_int(ارزښت, &ارزښت_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewBignumObj(&ارزښت_mp));
	} وإلا إذا (bool_flag) {
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} وإلا {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_سېټ_Obj_پایله(interp, Tcl_NewStringObj(obj->ځانګړتیاوې.at(attr_twine).as_string().c_str(), -1));
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

	خپلکاری mod_twine = yosys_design->twines().پیدا_کړه(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = تش_اشاره;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خپلکاری obj_twine = yosys_design->twines().پیدا_کړه(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, تش_اشاره);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, تش_اشاره);
	}

	إذا (!obj)
		ERROR("object not found")

	Tcl_سېټ_Obj_پایله(interp, Tcl_NewStringObj(std::منزل_string(obj->has_attribute(yosys_design->twines().پیدا_کړه(attr_id))).c_str(), -1));
	أرجع TCL_OK;
}

ساكن صحيح tcl_سېټ_attr(Client_معلومات, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
{
	صحيح i;
	منطقي mod_flag = خطأ, string_flag = خطأ, bool_flag = خطأ;
	منطقي true_flag = خطأ, false_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(ریښتیا)
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

	خپلکاری mod_twine = yosys_design->twines().پیدا_کړه(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = تش_اشاره;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خپلکاری obj_twine = yosys_design->twines().پیدا_کړه(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, تش_اشاره);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, تش_اشاره);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().جمع_کړه(attr_id);

	إذا (string_flag) {
		obj->سېټ_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ارزښت_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &ارزښت_mp))
			ERROR("non-integral value")

		إذا (!mp_int_منزل_const(&ارزښت_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.اندازه() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.اندازه() < 32)
				const_.extu(32);
		}

		obj->ځانګړتیاوې[attr_twine] = const_;
	} وإلا إذا (bool_flag) {
		obj->سېټ_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} وإلا إذا (true_flag) {
		obj->سېټ_bool_attribute(attr_twine, ریښتیا);
	} وإلا إذا (false_flag) {
		obj->سېټ_bool_attribute(attr_twine, خطأ);
	} وإلا {
		obj->ځانګړتیاوې[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
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

	خپلکاری mod_twine = yosys_design->twines().پیدا_کړه(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	خپلکاری cell_twine = yosys_design->twines().پیدا_کړه(cell_id);
	Cell* cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().پیدا_کړه(param_id);
	إذا (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ثابت RTLIL::Const &ارزښت = cell->getParam(param_twine);

	إذا (string_flag) {
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewStringObj(ارزښت.decode_string().c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		mp_int ارزښت_mp;
		إذا (!const_منزل_mp_int(ارزښت, &ارزښت_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewBignumObj(&ارزښت_mp));
	} وإلا {
		Tcl_سېټ_Obj_پایله(interp, Tcl_NewStringObj(ارزښت.as_string().c_str(), -1));
	}
	أرجع TCL_OK;
}

ساكن صحيح tcl_سېټ_param(Client_معلومات, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
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

	خپلکاری mod_twine = yosys_design->twines().پیدا_کړه(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	خپلکاری cell_twine = yosys_design->twines().پیدا_کړه(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().جمع_کړه(param_id);

	إذا (string_flag) {
		cell->سېټ_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ارزښت_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &ارزښت_mp))
			ERROR("non-integral value")

		إذا (!mp_int_منزل_const(&ارزښت_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.اندازه() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.اندازه() < 32)
				const_.extu(32);
		}

		cell->سېټ_Param(param_twine, const_);
	} وإلا {
		cell->سېټ_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	أرجع TCL_OK;
}

صحيح yosys_tcl_interp_init(Tcl_Interp *interp)
{
	إذا (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_پیغام(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_سېټ_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_سېټ_param, عدم, عدم);

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
#د_که_پای

YOSYS_NAMESPACE_پای_ټکی
