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

#اگر_تعریف_شده YOSYS_ENABLE_TCL
#ضمن <tcl.h>
#ضمن <tclTomMath.h>
#ضمن <tclTomMathDecls.h>
#پایان_اگر

YOSYS_NAMESPACE_BEGIN

#اگر_تعریف_شده YOSYS_ENABLE_TCL
منطقي yosys_tcl_repl_فعال = خطأ;

فراغ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_فعال = درست;
}

ساكن Tcl_Obj *json_مقصد_tcl(Tcl_Interp *interp, ثابت json11::Json &json)
{
	إذا (json.is_null())
		أرجع Tcl_NewStringObj("null", 4);
	وإلا إذا (json.is_string()) {
		خودکار string = json.string_مقدار();
		أرجع Tcl_NewStringObj(string.داده(), string.اندازه());
	} وإلا إذا (json.is_عدد()) {
		مزدوج مقدار = json.عدد_مقدار();
		مزدوج دور_val = std::nearbyint(مقدار);
		إذا (std::isfinite(دور_val) && مقدار == دور_val && مقدار >= LONG_MIN && مقدار < -مزدوج(LONG_MIN))
			أرجع Tcl_NewLongObj((دراز)دور_val);
		وإلا
			أرجع Tcl_NewDoubleObj(مقدار);
	} وإلا إذا (json.is_bool()) {
		أرجع Tcl_NewBooleanObj(json.bool_مقدار());
	} وإلا إذا (json.is_array()) {
		خودکار list = json.array_اقلام();
		Tcl_Obj *نتیجه = Tcl_NewListObj(list.اندازه(), اشاره_تهی);
		لكل (خودکار &قلم : list)
			Tcl_ListObjAppendElement(interp, نتیجه, json_مقصد_tcl(interp, قلم));
		أرجع نتیجه;
	} وإلا إذا (json.is_object()) {
		خودکار map = json.object_اقلام();
		Tcl_Obj *نتیجه = Tcl_NewListObj(map.اندازه() * 2, اشاره_تهی);
		لكل (خودکار &قلم : map) {
			Tcl_ListObjAppendElement(interp, نتیجه, Tcl_NewStringObj(قلم.اولی.داده(), قلم.اولی.اندازه()));
			Tcl_ListObjAppendElement(interp, نتیجه, json_مقصد_tcl(interp, قلم.دومی));
		}
		أرجع نتیجه;
	} وإلا {
		log_abort();
	}
}

ساكن صحيح tcl_yosys_cmd(Client_داده, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	std::وکتور<std::string> آرگومانها_2;
	لكل (صحيح i = 1; i < argc; i++)
		آرگومانها_2.افزودن_پشت(argv[i]);

	إذا (آرگومانها_2.اندازه() >= 1 && آرگومانها_2[0] == "-import") {
		لكل (خودکار &it : pass_ثبات) {
			std::string tcl_command_name = it.اولی;
			إذا (tcl_command_name == "proc")
				tcl_command_name = "procs";
			وإلا إذا (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			إذا (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				لگاریتم("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.اولی);
			} وإلا {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.اولی);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		أرجع TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	منطقي درون_repl = yosys_tcl_repl_فعال;
	خودکار guard = logger().error_throw_scope();
	حاول {
		إذا (آرگومانها_2.اندازه() == 1) {
			Pass::call(yosys_get_design(), آرگومانها_2[0]);
		} وإلا {
			Pass::call(yosys_get_design(), آرگومانها_2);
		}
	} امسك (log_cmd_error_exception) {
		إذا (درون_repl) {
			خودکار design = yosys_get_design();
			طالما (design->selection_پشته.اندازه() > 1)
				design->برداشت_selection();
			log_reset_پشته();
		}
		Tcl_مجموعه_نتیجه(interp, (حرف *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_فعال = درون_repl;
		أرجع TCL_ERROR;
	} امسك (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_فعال = درون_repl;

	خودکار &scratchpad = yosys_get_design()->scratchpad;
	خودکار نتیجه = scratchpad.پیدا_کن("result.json");
	إذا (نتیجه != scratchpad.انتها()) {
		std::string err;
		خودکار json = json11::Json::parse(نتیجه->دومی, err);
		إذا (err.خالی()) {
			Tcl_مجموعه_Obj_نتیجه(interp, json_مقصد_tcl(interp, json));
		} وإلا
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} وإلا إذا ((نتیجه = scratchpad.پیدا_کن("result.string")) != scratchpad.انتها()) {
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewStringObj(نتیجه->دومی.داده(), نتیجه->دومی.اندازه()));
	}

	أرجع TCL_OK;
}

#تعریف_کن FLAG(name) \
	إذا (!strcmp(argv[i], "-" #name)) { \
		name##_flag = درست; \
		استمر; \
	} \

#تعریف_کن FLAG2(name) \
	إذا (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = درست; \
		استمر; \
	} \

#تعریف_کن ERROR(str) \
	{ \
		Tcl_مجموعه_نتیجه(interp, (حرف *)(str), TCL_STATIC); \
		أرجع TCL_ERROR; \
	}

منطقي const_مقصد_mp_int(ثابت Const &a, mp_int *b, منطقي force_signed, منطقي force_unsigned)
{
	إذا (!a.is_fully_def())
		أرجع خطأ;

	إذا (mp_init(b))
		أرجع خطأ;

	منطقي negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.خالی() && (a.پشت() == RTLIL::S1);

	لكل (صحيح i = a.اندازه() - 1; i >= 0; i--) {
		إذا (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			أرجع خطأ;
		}

		إذا ((a[i] == RTLIL::S1) ^ negative) {
			إذا (mp_جمع_کن_d(b, 1, b)) {
				mp_clear(b);
				أرجع خطأ;
			}
		}
	}

	إذا (negative) {
		إذا (mp_جمع_کن_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			أرجع خطأ;
		}
	}

	أرجع درست;
}

منطقي mp_int_مقصد_const(mp_int *a, Const &b, منطقي is_signed)
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

	std::وکتور<بیعلامت حرف> buf;
	buf.resize(mp_unsigned_bin_اندازه(a));
	إذا (mp_مقصد_unsigned_bin(a, buf.داده()) != MP_OKAY)
		أرجع خطأ;

	Const::Builder b_بیتها(mp_شمار_بیتها(a) + is_signed);
	لكل (صحيح i = 0; i < mp_شمار_بیتها(a);) {
		لكل (صحيح j = 0; j < 8 && i < mp_شمار_بیتها(a); j++, i++) {
			منطقي bv = ((buf.پشت() & (1 << j)) != 0) ^ negative;
			b_بیتها.افزودن_پشت(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.برداشت_پشت();
	}

	إذا (is_signed) {
		b_بیتها.افزودن_پشت(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_بیتها.build();

	أرجع درست;
}

ساكن صحيح tcl_get_attr(Client_داده, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
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

	خودکار mod_twine = yosys_design->twines().پیدا_کن(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = اشاره_تهی;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خودکار obj_twine = yosys_design->twines().پیدا_کن(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, اشاره_تهی);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, اشاره_تهی);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().پیدا_کن(attr_id);

	إذا (string_flag) {
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &مقدار = obj->صفات.at(attr_twine);

		mp_int مقدار_mp;
		إذا (!const_مقصد_mp_int(مقدار, &مقدار_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewBignumObj(&مقدار_mp));
	} وإلا إذا (bool_flag) {
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} وإلا {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewStringObj(obj->صفات.at(attr_twine).as_string().c_str(), -1));
	}

	أرجع TCL_OK;
}

ساكن صحيح tcl_has_attr(Client_داده, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
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

	خودکار mod_twine = yosys_design->twines().پیدا_کن(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = اشاره_تهی;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خودکار obj_twine = yosys_design->twines().پیدا_کن(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, اشاره_تهی);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, اشاره_تهی);
	}

	إذا (!obj)
		ERROR("object not found")

	Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewStringObj(std::مقصد_string(obj->has_attribute(yosys_design->twines().پیدا_کن(attr_id))).c_str(), -1));
	أرجع TCL_OK;
}

ساكن صحيح tcl_مجموعه_attr(Client_داده, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
{
	صحيح i;
	منطقي mod_flag = خطأ, string_flag = خطأ, bool_flag = خطأ;
	منطقي true_flag = خطأ, false_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(درست)
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

	خودکار mod_twine = yosys_design->twines().پیدا_کن(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = اشاره_تهی;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		خودکار obj_twine = yosys_design->twines().پیدا_کن(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, اشاره_تهی);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, اشاره_تهی);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().جمع_کن(attr_id);

	إذا (string_flag) {
		obj->مجموعه_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int مقدار_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &مقدار_mp))
			ERROR("non-integral value")

		إذا (!mp_int_مقصد_const(&مقدار_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.اندازه() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.اندازه() < 32)
				const_.extu(32);
		}

		obj->صفات[attr_twine] = const_;
	} وإلا إذا (bool_flag) {
		obj->مجموعه_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} وإلا إذا (true_flag) {
		obj->مجموعه_bool_attribute(attr_twine, درست);
	} وإلا إذا (false_flag) {
		obj->مجموعه_bool_attribute(attr_twine, خطأ);
	} وإلا {
		obj->صفات[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	أرجع TCL_OK;
}

ساكن صحيح tcl_get_param(Client_داده, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
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

	خودکار mod_twine = yosys_design->twines().پیدا_کن(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	خودکار cell_twine = yosys_design->twines().پیدا_کن(cell_id);
	Cell* cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().پیدا_کن(param_id);
	إذا (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ثابت RTLIL::Const &مقدار = cell->getParam(param_twine);

	إذا (string_flag) {
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewStringObj(مقدار.decode_string().c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		mp_int مقدار_mp;
		إذا (!const_مقصد_mp_int(مقدار, &مقدار_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewBignumObj(&مقدار_mp));
	} وإلا {
		Tcl_مجموعه_Obj_نتیجه(interp, Tcl_NewStringObj(مقدار.as_string().c_str(), -1));
	}
	أرجع TCL_OK;
}

ساكن صحيح tcl_مجموعه_param(Client_داده, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
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

	خودکار mod_twine = yosys_design->twines().پیدا_کن(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	خودکار cell_twine = yosys_design->twines().پیدا_کن(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().جمع_کن(param_id);

	إذا (string_flag) {
		cell->مجموعه_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int مقدار_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &مقدار_mp))
			ERROR("non-integral value")

		إذا (!mp_int_مقصد_const(&مقدار_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.اندازه() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.اندازه() < 32)
				const_.extu(32);
		}

		cell->مجموعه_Param(param_twine, const_);
	} وإلا {
		cell->مجموعه_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	أرجع TCL_OK;
}

صحيح yosys_tcl_interp_init(Tcl_Interp *interp)
{
	إذا (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_پیام(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_مجموعه_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_مجموعه_param, عدم, عدم);

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
#پایان_اگر

YOSYS_NAMESPACE_انتها
