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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "libs/json11/json11.hpp"

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
#अन्तर्भाव <tcl.h>
#अन्तर्भाव <tclTomMath.h>
#अन्तर्भाव <tclTomMathDecls.h>
#यदि_अन्तः

YOSYS_NAMESPACE_BEGIN

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
तार्किक yosys_tcl_repl_सक्रिय = असत्य;

शून्य yosys_tcl_activate_repl()
{
	yosys_tcl_repl_सक्रिय = सत्य;
}

स्थिर Tcl_Obj *json_गन्तव्यम्_tcl(Tcl_Interp *interp, ध्रुव json11::Json &json)
{
	यदि (json.is_null())
		प्रत्यागम Tcl_NewStringObj("null", 4);
	अन्यथा यदि (json.is_string()) {
		स्वचालित string = json.string_मानम्();
		प्रत्यागम Tcl_NewStringObj(string.दत्त(), string.आकार());
	} अन्यथा यदि (json.is_सङ्ख्या()) {
		द्विगुण मानम् = json.सङ्ख्या_मानम्();
		द्विगुण आवर्तनम्_val = std::nearbyint(मानम्);
		यदि (std::isfinite(आवर्तनम्_val) && मानम् == आवर्तनम्_val && मानम् >= LONG_MIN && मानम् < -द्विगुण(LONG_MIN))
			प्रत्यागम Tcl_NewLongObj((दीर्घ)आवर्तनम्_val);
		अन्यथा
			प्रत्यागम Tcl_NewDoubleObj(मानम्);
	} अन्यथा यदि (json.is_bool()) {
		प्रत्यागम Tcl_NewBooleanObj(json.bool_मानम्());
	} अन्यथा यदि (json.is_array()) {
		स्वचालित list = json.array_वस्तूनि();
		Tcl_Obj *परिणाम = Tcl_NewListObj(list.आकार(), शून्यसूचकः);
		प्रति (स्वचालित &item : list)
			Tcl_ListObjAppendElement(interp, परिणाम, json_गन्तव्यम्_tcl(interp, item));
		प्रत्यागम परिणाम;
	} अन्यथा यदि (json.is_object()) {
		स्वचालित map = json.object_वस्तूनि();
		Tcl_Obj *परिणाम = Tcl_NewListObj(map.आकार() * 2, शून्यसूचकः);
		प्रति (स्वचालित &item : map) {
			Tcl_ListObjAppendElement(interp, परिणाम, Tcl_NewStringObj(item.first.दत्त(), item.first.आकार()));
			Tcl_ListObjAppendElement(interp, परिणाम, json_गन्तव्यम्_tcl(interp, item.second));
		}
		प्रत्यागम परिणाम;
	} अन्यथा {
		log_abort();
	}
}

स्थिर पूर्णाङ्क tcl_yosys_cmd(Client_दत्त, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	std::सदिशम्<std::string> तर्काः_2;
	प्रति (पूर्णाङ्क i = 1; i < argc; i++)
		तर्काः_2.प्रक्षिप_पृष्ठभागः(argv[i]);

	यदि (तर्काः_2.आकार() >= 1 && तर्काः_2[0] == "-import") {
		प्रति (स्वचालित &it : pass_पञ्जिकाकोशः) {
			std::string tcl_command_name = it.first;
			यदि (tcl_command_name == "proc")
				tcl_command_name = "procs";
			अन्यथा यदि (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			यदि (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				लघुगणकः("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} अन्यथा {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		प्रत्यागम TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	तार्किक अन्तर्_repl = yosys_tcl_repl_सक्रिय;
	स्वचालित guard = logger().error_throw_scope();
	यत्न {
		यदि (तर्काः_2.आकार() == 1) {
			Pass::call(yosys_get_design(), तर्काः_2[0]);
		} अन्यथा {
			Pass::call(yosys_get_design(), तर्काः_2);
		}
	} गृहाण (log_cmd_error_exception) {
		यदि (अन्तर्_repl) {
			स्वचालित design = yosys_get_design();
			यावत् (design->selection_राशिः.आकार() > 1)
				design->निष्कासय_selection();
			log_reset_राशिः();
		}
		Tcl_समुच्चयः_परिणाम(interp, (वर्ण *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_सक्रिय = अन्तर्_repl;
		प्रत्यागम TCL_ERROR;
	} गृहाण (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_सक्रिय = अन्तर्_repl;

	स्वचालित &scratchpad = yosys_get_design()->scratchpad;
	स्वचालित परिणाम = scratchpad.find("result.json");
	यदि (परिणाम != scratchpad.अन्तः()) {
		std::string err;
		स्वचालित json = json11::Json::parse(परिणाम->second, err);
		यदि (err.रिक्तानि()) {
			Tcl_समुच्चयः_Obj_परिणाम(interp, json_गन्तव्यम्_tcl(interp, json));
		} अन्यथा
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} अन्यथा यदि ((परिणाम = scratchpad.find("result.string")) != scratchpad.अन्तः()) {
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewStringObj(परिणाम->second.दत्त(), परिणाम->second.आकार()));
	}

	प्रत्यागम TCL_OK;
}

#परिभाषय FLAG(name) \
	यदि (!वाक्यं_तुलय(argv[i], "-" #name)) { \
		name##_flag = सत्य; \
		अनुवर्त; \
	} \

#परिभाषय FLAG2(name) \
	यदि (!वाक्यं_तुलय(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = सत्य; \
		अनुवर्त; \
	} \

#परिभाषय ERROR(str) \
	{ \
		Tcl_समुच्चयः_परिणाम(interp, (वर्ण *)(str), TCL_STATIC); \
		प्रत्यागम TCL_ERROR; \
	}

तार्किक const_गन्तव्यम्_mp_int(ध्रुव Const &a, mp_int *b, तार्किक force_signed, तार्किक force_unsigned)
{
	यदि (!a.is_fully_def())
		प्रत्यागम असत्य;

	यदि (mp_init(b))
		प्रत्यागम असत्य;

	तार्किक negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.रिक्तानि() && (a.पृष्ठभागः() == RTLIL::S1);

	प्रति (पूर्णाङ्क i = a.आकार() - 1; i >= 0; i--) {
		यदि (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			प्रत्यागम असत्य;
		}

		यदि ((a[i] == RTLIL::S1) ^ negative) {
			यदि (mp_सङ्कलय_d(b, 1, b)) {
				mp_clear(b);
				प्रत्यागम असत्य;
			}
		}
	}

	यदि (negative) {
		यदि (mp_सङ्कलय_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			प्रत्यागम असत्य;
		}
	}

	प्रत्यागम सत्य;
}

तार्किक mp_int_गन्तव्यम्_const(mp_int *a, Const &b, तार्किक is_signed)
{
	तार्किक negative = (mp_cmp_d(a, 0) == MP_LT);
	यदि (negative && !is_signed)
		प्रत्यागम असत्य;

	यदि (negative) {
		यदि (mp_neg(a, a) != MP_OKAY)
			प्रत्यागम असत्य;
		यदि (mp_sub_d(a, 1, a) != MP_OKAY)
			प्रत्यागम असत्य;
	}

	std::सदिशम्<अचिह्नित वर्ण> buf;
	buf.resize(mp_unsigned_bin_आकार(a));
	यदि (mp_गन्तव्यम्_unsigned_bin(a, buf.दत्त()) != MP_OKAY)
		प्रत्यागम असत्य;

	Const::Builder b_द्वयङ्काः(mp_गणनम्_द्वयङ्काः(a) + is_signed);
	प्रति (पूर्णाङ्क i = 0; i < mp_गणनम्_द्वयङ्काः(a);) {
		प्रति (पूर्णाङ्क j = 0; j < 8 && i < mp_गणनम्_द्वयङ्काः(a); j++, i++) {
			तार्किक bv = ((buf.पृष्ठभागः() & (1 << j)) != 0) ^ negative;
			b_द्वयङ्काः.प्रक्षिप_पृष्ठभागः(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.निष्कासय_पृष्ठभागः();
	}

	यदि (is_signed) {
		b_द्वयङ्काः.प्रक्षिप_पृष्ठभागः(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_द्वयङ्काः.build();

	प्रत्यागम सत्य;
}

स्थिर पूर्णाङ्क tcl_get_attr(Client_दत्त, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	पूर्णाङ्क i;
	तार्किक mod_flag = असत्य, string_flag = असत्य, bool_flag = असत्य;
	तार्किक int_flag = असत्य, sint_flag = असत्य, uint_flag = असत्य;
	प्रति (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(पूर्णाङ्क)
		FLAG(sint)
		FLAG(uint)
		FLAG(तार्किक)
		भङ्ग;
	}

	यदि ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	यदि (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	स्वचालित mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	यदि (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = शून्यसूचकः;
	यदि (mod_flag) {
		obj = mod;
	} अन्यथा {
		स्वचालित obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		यदि (!obj)
			obj = mod->memories.at(obj_twine, शून्यसूचकः);
		यदि (!obj)
			obj = mod->cell(obj_twine);
		यदि (!obj)
			obj = mod->processes.at(obj_twine, शून्यसूचकः);
	}

	यदि (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	यदि (string_flag) {
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} अन्यथा यदि (int_flag || uint_flag || sint_flag) {
		यदि (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &मानम् = obj->गुणाः.at(attr_twine);

		mp_int मानम्_mp;
		यदि (!const_गन्तव्यम्_mp_int(मानम्, &मानम्_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewBignumObj(&मानम्_mp));
	} अन्यथा यदि (bool_flag) {
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} अन्यथा {
		यदि (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewStringObj(obj->गुणाः.at(attr_twine).as_string().c_str(), -1));
	}

	प्रत्यागम TCL_OK;
}

स्थिर पूर्णाङ्क tcl_has_attr(Client_दत्त, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	पूर्णाङ्क i;
	तार्किक mod_flag = असत्य;
	प्रति (i = 1; i < argc; i++) {
		FLAG(mod)
		भङ्ग;
	}

	यदि ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	यदि (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	स्वचालित mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	यदि (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = शून्यसूचकः;
	यदि (mod_flag) {
		obj = mod;
	} अन्यथा {
		स्वचालित obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		यदि (!obj)
			obj = mod->memories.at(obj_twine, शून्यसूचकः);
		यदि (!obj)
			obj = mod->cell(obj_twine);
		यदि (!obj)
			obj = mod->processes.at(obj_twine, शून्यसूचकः);
	}

	यदि (!obj)
		ERROR("object not found")

	Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewStringObj(std::गन्तव्यम्_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	प्रत्यागम TCL_OK;
}

स्थिर पूर्णाङ्क tcl_समुच्चयः_attr(Client_दत्त, Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *ध्रुव objv[])
{
	पूर्णाङ्क i;
	तार्किक mod_flag = असत्य, string_flag = असत्य, bool_flag = असत्य;
	तार्किक true_flag = असत्य, false_flag = असत्य, sint_flag = असत्य, uint_flag = असत्य;
	प्रति (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(सत्य)
		FLAG2(असत्य)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(तार्किक)
		भङ्ग;
	}

	यदि ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	यदि (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	स्वचालित mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	यदि (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = शून्यसूचकः;
	यदि (mod_flag) {
		obj = mod;
	} अन्यथा {
		स्वचालित obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		यदि (!obj)
			obj = mod->memories.at(obj_twine, शून्यसूचकः);
		यदि (!obj)
			obj = mod->cell(obj_twine);
		यदि (!obj)
			obj = mod->processes.at(obj_twine, शून्यसूचकः);
	}

	यदि (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().सङ्कलय(attr_id);

	यदि (string_flag) {
		obj->समुच्चयः_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} अन्यथा यदि (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int मानम्_mp;

		यदि (Tcl_TakeBignumFromObj(interp, objv[i++], &मानम्_mp))
			ERROR("non-integral value")

		यदि (!mp_int_गन्तव्यम्_const(&मानम्_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		यदि (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			यदि (const_.आकार() < 32)
				const_.exts(32);
		} अन्यथा {
			यदि (const_.आकार() < 32)
				const_.extu(32);
		}

		obj->गुणाः[attr_twine] = const_;
	} अन्यथा यदि (bool_flag) {
		obj->समुच्चयः_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} अन्यथा यदि (true_flag) {
		obj->समुच्चयः_bool_attribute(attr_twine, सत्य);
	} अन्यथा यदि (false_flag) {
		obj->समुच्चयः_bool_attribute(attr_twine, असत्य);
	} अन्यथा {
		obj->गुणाः[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	प्रत्यागम TCL_OK;
}

स्थिर पूर्णाङ्क tcl_get_param(Client_दत्त, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	पूर्णाङ्क i;
	तार्किक string_flag = असत्य;
	तार्किक int_flag = असत्य, sint_flag = असत्य, uint_flag = असत्य;
	प्रति (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(पूर्णाङ्क)
		FLAG(sint)
		FLAG(uint)
		भङ्ग;
	}

	यदि ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	स्वचालित mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	यदि (!mod)
		ERROR("module not found")

	स्वचालित cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	यदि (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	यदि (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ध्रुव RTLIL::Const &मानम् = cell->getParam(param_twine);

	यदि (string_flag) {
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewStringObj(मानम्.decode_string().c_str(), -1));
	} अन्यथा यदि (int_flag || uint_flag || sint_flag) {
		mp_int मानम्_mp;
		यदि (!const_गन्तव्यम्_mp_int(मानम्, &मानम्_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewBignumObj(&मानम्_mp));
	} अन्यथा {
		Tcl_समुच्चयः_Obj_परिणाम(interp, Tcl_NewStringObj(मानम्.as_string().c_str(), -1));
	}
	प्रत्यागम TCL_OK;
}

स्थिर पूर्णाङ्क tcl_समुच्चयः_param(Client_दत्त, Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *ध्रुव objv[])
{
	पूर्णाङ्क i;
	तार्किक string_flag = असत्य, sint_flag = असत्य, uint_flag = असत्य;
	प्रति (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		भङ्ग;
	}

	यदि ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	स्वचालित mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	यदि (!mod)
		ERROR("module not found")

	स्वचालित cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	यदि (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().सङ्कलय(param_id);

	यदि (string_flag) {
		cell->समुच्चयः_Param(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} अन्यथा यदि (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int मानम्_mp;

		यदि (Tcl_TakeBignumFromObj(interp, objv[i++], &मानम्_mp))
			ERROR("non-integral value")

		यदि (!mp_int_गन्तव्यम्_const(&मानम्_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		यदि (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			यदि (const_.आकार() < 32)
				const_.exts(32);
		} अन्यथा {
			यदि (const_.आकार() < 32)
				const_.extu(32);
		}

		cell->समुच्चयः_Param(param_twine, const_);
	} अन्यथा {
		cell->समुच्चयः_Param(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	प्रत्यागम TCL_OK;
}

पूर्णाङ्क yosys_tcl_interp_init(Tcl_Interp *interp)
{
	यदि (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_सन्देशः(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, रिक्त, रिक्त);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, रिक्त, रिक्त);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, रिक्त, रिक्त);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_समुच्चयः_attr, रिक्त, रिक्त);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, रिक्त, रिक्त);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_समुच्चयः_param, रिक्त, रिक्त);

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

	प्रत्यागम TCL_OK ;
}
#यदि_अन्तः

YOSYS_NAMESPACE_अन्तः
