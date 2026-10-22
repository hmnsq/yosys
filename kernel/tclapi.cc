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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ "libs/json11/json11.hpp"

#నిర్వచితమైతే YOSYS_ENABLE_TCL
#అన్తర్భావ <tcl.h>
#అన్తర్భావ <tclTomMath.h>
#అన్తర్భావ <tclTomMathDecls.h>
#అయితే_ముగింపు

YOSYS_NAMESPACE_BEGIN

#నిర్వచితమైతే YOSYS_ENABLE_TCL
తార్కిక yosys_tcl_repl_చురుకైన = అబద్ధం;

శూన్య yosys_tcl_activate_repl()
{
	yosys_tcl_repl_చురుకైన = నిజం;
}

స్థిర Tcl_Obj *json_గమ్యం_tcl(Tcl_Interp *interp, స్థిరరాశి json11::Json &json)
{
	ఒకవేళ (json.is_null())
		తిరిగి Tcl_NewStringObj("null", 4);
	లేకపోతే ఒకవేళ (json.is_string()) {
		స్వయంచాలక string = json.string_విలువ();
		తిరిగి Tcl_NewStringObj(string.దత్తాంశం(), string.కొలత());
	} లేకపోతే ఒకవేళ (json.is_సంఖ్య()) {
		ద్విగుణ విలువ = json.సంఖ్య_విలువ();
		ద్విగుణ విడత_val = std::nearbyint(విలువ);
		ఒకవేళ (std::isfinite(విడత_val) && విలువ == విడత_val && విలువ >= LONG_MIN && విలువ < -ద్విగుణ(LONG_MIN))
			తిరిగి Tcl_NewLongObj((దీర్ఘ)విడత_val);
		లేకపోతే
			తిరిగి Tcl_NewDoubleObj(విలువ);
	} లేకపోతే ఒకవేళ (json.is_bool()) {
		తిరిగి Tcl_NewBooleanObj(json.bool_విలువ());
	} లేకపోతే ఒకవేళ (json.is_array()) {
		స్వయంచాలక list = json.array_వస్తువులు();
		Tcl_Obj *ఫలితం = Tcl_NewListObj(list.కొలత(), శూన్యసూచిక);
		కొరకు (స్వయంచాలక &అంశం : list)
			Tcl_ListObjAppendElement(interp, ఫలితం, json_గమ్యం_tcl(interp, అంశం));
		తిరిగి ఫలితం;
	} లేకపోతే ఒకవేళ (json.is_object()) {
		స్వయంచాలక map = json.object_వస్తువులు();
		Tcl_Obj *ఫలితం = Tcl_NewListObj(map.కొలత() * 2, శూన్యసూచిక);
		కొరకు (స్వయంచాలక &అంశం : map) {
			Tcl_ListObjAppendElement(interp, ఫలితం, Tcl_NewStringObj(అంశం.మొదటి.దత్తాంశం(), అంశం.మొదటి.కొలత()));
			Tcl_ListObjAppendElement(interp, ఫలితం, json_గమ్యం_tcl(interp, అంశం.రెండవ));
		}
		తిరిగి ఫలితం;
	} లేకపోతే {
		log_abort();
	}
}

స్థిర పూర్ణాంకం tcl_yosys_cmd(Client_దత్తాంశం, Tcl_Interp *interp, పూర్ణాంకం argc, స్థిరరాశి అక్షరం *argv[])
{
	std::సదిశ<std::string> వాదనలు_2;
	కొరకు (పూర్ణాంకం i = 1; i < argc; i++)
		వాదనలు_2.నెట్టు_వెనుక(argv[i]);

	ఒకవేళ (వాదనలు_2.కొలత() >= 1 && వాదనలు_2[0] == "-import") {
		కొరకు (స్వయంచాలక &it : pass_రిజిస్టరు) {
			std::string tcl_command_name = it.మొదటి;
			ఒకవేళ (tcl_command_name == "proc")
				tcl_command_name = "procs";
			లేకపోతే ఒకవేళ (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			ఒకవేళ (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				సంవర్గమానం("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.మొదటి);
			} లేకపోతే {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.మొదటి);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		తిరిగి TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	తార్కిక లోపల_repl = yosys_tcl_repl_చురుకైన;
	స్వయంచాలక guard = logger().error_throw_scope();
	ప్రయత్నించు {
		ఒకవేళ (వాదనలు_2.కొలత() == 1) {
			Pass::call(yosys_get_design(), వాదనలు_2[0]);
		} లేకపోతే {
			Pass::call(yosys_get_design(), వాదనలు_2);
		}
	} పట్టుకో (log_cmd_error_exception) {
		ఒకవేళ (లోపల_repl) {
			స్వయంచాలక design = yosys_get_design();
			అయితే (design->selection_దొంతర.కొలత() > 1)
				design->తీయి_selection();
			log_reset_దొంతర();
		}
		Tcl_సమితి_ఫలితం(interp, (అక్షరం *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_చురుకైన = లోపల_repl;
		తిరిగి TCL_ERROR;
	} పట్టుకో (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_చురుకైన = లోపల_repl;

	స్వయంచాలక &scratchpad = yosys_get_design()->scratchpad;
	స్వయంచాలక ఫలితం = scratchpad.కనుగొను("result.json");
	ఒకవేళ (ఫలితం != scratchpad.చివర()) {
		std::string err;
		స్వయంచాలక json = json11::Json::parse(ఫలితం->రెండవ, err);
		ఒకవేళ (err.ఖాళీలు()) {
			Tcl_సమితి_Obj_ఫలితం(interp, json_గమ్యం_tcl(interp, json));
		} లేకపోతే
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} లేకపోతే ఒకవేళ ((ఫలితం = scratchpad.కనుగొను("result.string")) != scratchpad.చివర()) {
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewStringObj(ఫలితం->రెండవ.దత్తాంశం(), ఫలితం->రెండవ.కొలత()));
	}

	తిరిగి TCL_OK;
}

#నిర్వచించు FLAG(name) \
	ఒకవేళ (!strcmp(argv[i], "-" #name)) { \
		name##_flag = నిజం; \
		కొనసాగు; \
	} \

#నిర్వచించు FLAG2(name) \
	ఒకవేళ (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = నిజం; \
		కొనసాగు; \
	} \

#నిర్వచించు ERROR(str) \
	{ \
		Tcl_సమితి_ఫలితం(interp, (అక్షరం *)(str), TCL_STATIC); \
		తిరిగి TCL_ERROR; \
	}

తార్కిక const_గమ్యం_mp_int(స్థిరరాశి Const &a, mp_int *b, తార్కిక force_signed, తార్కిక force_unsigned)
{
	ఒకవేళ (!a.is_fully_def())
		తిరిగి అబద్ధం;

	ఒకవేళ (mp_init(b))
		తిరిగి అబద్ధం;

	తార్కిక negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.ఖాళీలు() && (a.వెనుక() == RTLIL::S1);

	కొరకు (పూర్ణాంకం i = a.కొలత() - 1; i >= 0; i--) {
		ఒకవేళ (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			తిరిగి అబద్ధం;
		}

		ఒకవేళ ((a[i] == RTLIL::S1) ^ negative) {
			ఒకవేళ (mp_కూడు_d(b, 1, b)) {
				mp_clear(b);
				తిరిగి అబద్ధం;
			}
		}
	}

	ఒకవేళ (negative) {
		ఒకవేళ (mp_కూడు_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			తిరిగి అబద్ధం;
		}
	}

	తిరిగి నిజం;
}

తార్కిక mp_int_గమ్యం_const(mp_int *a, Const &b, తార్కిక is_signed)
{
	తార్కిక negative = (mp_cmp_d(a, 0) == MP_LT);
	ఒకవేళ (negative && !is_signed)
		తిరిగి అబద్ధం;

	ఒకవేళ (negative) {
		ఒకవేళ (mp_neg(a, a) != MP_OKAY)
			తిరిగి అబద్ధం;
		ఒకవేళ (mp_sub_d(a, 1, a) != MP_OKAY)
			తిరిగి అబద్ధం;
	}

	std::సదిశ<గుర్తులేని అక్షరం> buf;
	buf.resize(mp_unsigned_bin_కొలత(a));
	ఒకవేళ (mp_గమ్యం_unsigned_bin(a, buf.దత్తాంశం()) != MP_OKAY)
		తిరిగి అబద్ధం;

	Const::Builder b_బిట్లు(mp_లెక్క_బిట్లు(a) + is_signed);
	కొరకు (పూర్ణాంకం i = 0; i < mp_లెక్క_బిట్లు(a);) {
		కొరకు (పూర్ణాంకం j = 0; j < 8 && i < mp_లెక్క_బిట్లు(a); j++, i++) {
			తార్కిక bv = ((buf.వెనుక() & (1 << j)) != 0) ^ negative;
			b_బిట్లు.నెట్టు_వెనుక(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.తీయి_వెనుక();
	}

	ఒకవేళ (is_signed) {
		b_బిట్లు.నెట్టు_వెనుక(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_బిట్లు.build();

	తిరిగి నిజం;
}

స్థిర పూర్ణాంకం tcl_get_attr(Client_దత్తాంశం, Tcl_Interp *interp, పూర్ణాంకం argc, స్థిరరాశి అక్షరం *argv[])
{
	పూర్ణాంకం i;
	తార్కిక mod_flag = అబద్ధం, string_flag = అబద్ధం, bool_flag = అబద్ధం;
	తార్కిక int_flag = అబద్ధం, sint_flag = అబద్ధం, uint_flag = అబద్ధం;
	కొరకు (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(పూర్ణాంకం)
		FLAG(sint)
		FLAG(uint)
		FLAG(తార్కిక)
		ఆపు;
	}

	ఒకవేళ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ఒకవేళ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	స్వయంచాలక mod_twine = yosys_design->twines().కనుగొను(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ఒకవేళ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = శూన్యసూచిక;
	ఒకవేళ (mod_flag) {
		obj = mod;
	} లేకపోతే {
		స్వయంచాలక obj_twine = yosys_design->twines().కనుగొను(obj_id);
		obj = mod->wire(obj_twine);
		ఒకవేళ (!obj)
			obj = mod->memories.at(obj_twine, శూన్యసూచిక);
		ఒకవేళ (!obj)
			obj = mod->cell(obj_twine);
		ఒకవేళ (!obj)
			obj = mod->processes.at(obj_twine, శూన్యసూచిక);
	}

	ఒకవేళ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().కనుగొను(attr_id);

	ఒకవేళ (string_flag) {
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} లేకపోతే ఒకవేళ (int_flag || uint_flag || sint_flag) {
		ఒకవేళ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &విలువ = obj->లక్షణాలు.at(attr_twine);

		mp_int విలువ_mp;
		ఒకవేళ (!const_గమ్యం_mp_int(విలువ, &విలువ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewBignumObj(&విలువ_mp));
	} లేకపోతే ఒకవేళ (bool_flag) {
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} లేకపోతే {
		ఒకవేళ (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewStringObj(obj->లక్షణాలు.at(attr_twine).as_string().c_str(), -1));
	}

	తిరిగి TCL_OK;
}

స్థిర పూర్ణాంకం tcl_has_attr(Client_దత్తాంశం, Tcl_Interp *interp, పూర్ణాంకం argc, స్థిరరాశి అక్షరం *argv[])
{
	పూర్ణాంకం i;
	తార్కిక mod_flag = అబద్ధం;
	కొరకు (i = 1; i < argc; i++) {
		FLAG(mod)
		ఆపు;
	}

	ఒకవేళ ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ఒకవేళ (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	స్వయంచాలక mod_twine = yosys_design->twines().కనుగొను(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ఒకవేళ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = శూన్యసూచిక;
	ఒకవేళ (mod_flag) {
		obj = mod;
	} లేకపోతే {
		స్వయంచాలక obj_twine = yosys_design->twines().కనుగొను(obj_id);
		obj = mod->wire(obj_twine);
		ఒకవేళ (!obj)
			obj = mod->memories.at(obj_twine, శూన్యసూచిక);
		ఒకవేళ (!obj)
			obj = mod->cell(obj_twine);
		ఒకవేళ (!obj)
			obj = mod->processes.at(obj_twine, శూన్యసూచిక);
	}

	ఒకవేళ (!obj)
		ERROR("object not found")

	Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewStringObj(std::గమ్యం_string(obj->has_attribute(yosys_design->twines().కనుగొను(attr_id))).c_str(), -1));
	తిరిగి TCL_OK;
}

స్థిర పూర్ణాంకం tcl_సమితి_attr(Client_దత్తాంశం, Tcl_Interp *interp, పూర్ణాంకం objc, Tcl_Obj *స్థిరరాశి objv[])
{
	పూర్ణాంకం i;
	తార్కిక mod_flag = అబద్ధం, string_flag = అబద్ధం, bool_flag = అబద్ధం;
	తార్కిక true_flag = అబద్ధం, false_flag = అబద్ధం, sint_flag = అబద్ధం, uint_flag = అబద్ధం;
	కొరకు (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(నిజం)
		FLAG2(అబద్ధం)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(తార్కిక)
		ఆపు;
	}

	ఒకవేళ ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	ఒకవేళ (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	స్వయంచాలక mod_twine = yosys_design->twines().కనుగొను(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ఒకవేళ (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = శూన్యసూచిక;
	ఒకవేళ (mod_flag) {
		obj = mod;
	} లేకపోతే {
		స్వయంచాలక obj_twine = yosys_design->twines().కనుగొను(obj_id);
		obj = mod->wire(obj_twine);
		ఒకవేళ (!obj)
			obj = mod->memories.at(obj_twine, శూన్యసూచిక);
		ఒకవేళ (!obj)
			obj = mod->cell(obj_twine);
		ఒకవేళ (!obj)
			obj = mod->processes.at(obj_twine, శూన్యసూచిక);
	}

	ఒకవేళ (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().కూడు(attr_id);

	ఒకవేళ (string_flag) {
		obj->సమితి_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} లేకపోతే ఒకవేళ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int విలువ_mp;

		ఒకవేళ (Tcl_TakeBignumFromObj(interp, objv[i++], &విలువ_mp))
			ERROR("non-integral value")

		ఒకవేళ (!mp_int_గమ్యం_const(&విలువ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ఒకవేళ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ఒకవేళ (const_.కొలత() < 32)
				const_.exts(32);
		} లేకపోతే {
			ఒకవేళ (const_.కొలత() < 32)
				const_.extu(32);
		}

		obj->లక్షణాలు[attr_twine] = const_;
	} లేకపోతే ఒకవేళ (bool_flag) {
		obj->సమితి_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} లేకపోతే ఒకవేళ (true_flag) {
		obj->సమితి_bool_attribute(attr_twine, నిజం);
	} లేకపోతే ఒకవేళ (false_flag) {
		obj->సమితి_bool_attribute(attr_twine, అబద్ధం);
	} లేకపోతే {
		obj->లక్షణాలు[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	తిరిగి TCL_OK;
}

స్థిర పూర్ణాంకం tcl_get_param(Client_దత్తాంశం, Tcl_Interp *interp, పూర్ణాంకం argc, స్థిరరాశి అక్షరం *argv[])
{
	పూర్ణాంకం i;
	తార్కిక string_flag = అబద్ధం;
	తార్కిక int_flag = అబద్ధం, sint_flag = అబద్ధం, uint_flag = అబద్ధం;
	కొరకు (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(పూర్ణాంకం)
		FLAG(sint)
		FLAG(uint)
		ఆపు;
	}

	ఒకవేళ ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	స్వయంచాలక mod_twine = yosys_design->twines().కనుగొను(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ఒకవేళ (!mod)
		ERROR("module not found")

	స్వయంచాలక cell_twine = yosys_design->twines().కనుగొను(cell_id);
	Cell* cell = mod->cell(cell_twine);
	ఒకవేళ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().కనుగొను(param_id);
	ఒకవేళ (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	స్థిరరాశి RTLIL::Const &విలువ = cell->getParam(param_twine);

	ఒకవేళ (string_flag) {
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewStringObj(విలువ.decode_string().c_str(), -1));
	} లేకపోతే ఒకవేళ (int_flag || uint_flag || sint_flag) {
		mp_int విలువ_mp;
		ఒకవేళ (!const_గమ్యం_mp_int(విలువ, &విలువ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewBignumObj(&విలువ_mp));
	} లేకపోతే {
		Tcl_సమితి_Obj_ఫలితం(interp, Tcl_NewStringObj(విలువ.as_string().c_str(), -1));
	}
	తిరిగి TCL_OK;
}

స్థిర పూర్ణాంకం tcl_సమితి_param(Client_దత్తాంశం, Tcl_Interp *interp, పూర్ణాంకం objc, Tcl_Obj *స్థిరరాశి objv[])
{
	పూర్ణాంకం i;
	తార్కిక string_flag = అబద్ధం, sint_flag = అబద్ధం, uint_flag = అబద్ధం;
	కొరకు (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		ఆపు;
	}

	ఒకవేళ ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	స్వయంచాలక mod_twine = yosys_design->twines().కనుగొను(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ఒకవేళ (!mod)
		ERROR("module not found")

	స్వయంచాలక cell_twine = yosys_design->twines().కనుగొను(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	ఒకవేళ (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().కూడు(param_id);

	ఒకవేళ (string_flag) {
		cell->సమితి_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} లేకపోతే ఒకవేళ (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int విలువ_mp;

		ఒకవేళ (Tcl_TakeBignumFromObj(interp, objv[i++], &విలువ_mp))
			ERROR("non-integral value")

		ఒకవేళ (!mp_int_గమ్యం_const(&విలువ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ఒకవేళ (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ఒకవేళ (const_.కొలత() < 32)
				const_.exts(32);
		} లేకపోతే {
			ఒకవేళ (const_.కొలత() < 32)
				const_.extu(32);
		}

		cell->సమితి_Param(param_twine, const_);
	} లేకపోతే {
		cell->సమితి_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	తిరిగి TCL_OK;
}

పూర్ణాంకం yosys_tcl_interp_init(Tcl_Interp *interp)
{
	ఒకవేళ (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_సందేశం(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ఖాళీ, ఖాళీ);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ఖాళీ, ఖాళీ);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ఖాళీ, ఖాళీ);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_సమితి_attr, ఖాళీ, ఖాళీ);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ఖాళీ, ఖాళీ);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_సమితి_param, ఖాళీ, ఖాళీ);

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

	తిరిగి TCL_OK ;
}
#అయితే_ముగింపు

YOSYS_NAMESPACE_చివర
