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

#ifdef YOSYS_ENABLE_TCL
#अन्तर्भाव <tcl.h>
#अन्तर्भाव <tclTomMath.h>
#अन्तर्भाव <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
तार्किक yosys_tcl_repl_active = मिच्छा;

शून्य yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = सच्च;
}

स्थिर Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ध्रुव json11::Json &json)
{
	सचे (json.is_null())
		पटिनिवत्त Tcl_NewStringObj("null", 4);
	अञ्ञथा सचे (json.is_string()) {
		auto string = json.string_मानम्();
		पटिनिवत्त Tcl_NewStringObj(string.data(), string.size());
	} अञ्ञथा सचे (json.is_सङ्ख्या()) {
		द्विगुण मानम् = json.सङ्ख्या_मानम्();
		द्विगुण round_val = std::nearbyint(मानम्);
		सचे (std::isfinite(round_val) && मानम् == round_val && मानम् >= LONG_MIN && मानम् < -द्विगुण(LONG_MIN))
			पटिनिवत्त Tcl_NewLongObj((long)round_val);
		अञ्ञथा
			पटिनिवत्त Tcl_NewDoubleObj(मानम्);
	} अञ्ञथा सचे (json.is_bool()) {
		पटिनिवत्त Tcl_NewBooleanObj(json.bool_मानम्());
	} अञ्ञथा सचे (json.is_array()) {
		auto list = json.array_वस्तूनि();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		अत्थाय (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		पटिनिवत्त result;
	} अञ्ञथा सचे (json.is_object()) {
		auto map = json.object_वस्तूनि();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		अत्थाय (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		पटिनिवत्त result;
	} अञ्ञथा {
		log_abort();
	}
}

स्थिर पूर्णाङ्क tcl_yosys_cmd(ClientData, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	std::vector<std::string> args;
	अत्थाय (पूर्णाङ्क i = 1; i < argc; i++)
		args.प्रक्षिप_back(argv[i]);

	सचे (args.size() >= 1 && args[0] == "-import") {
		अत्थाय (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			सचे (tcl_command_name == "proc")
				tcl_command_name = "procs";
			अञ्ञथा सचे (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			सचे (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} अञ्ञथा {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		पटिनिवत्त TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	तार्किक in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	यत्न {
		सचे (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} अञ्ञथा {
			Pass::call(yosys_get_design(), args);
		}
	} गृहाण (log_cmd_error_exception) {
		सचे (in_repl) {
			auto design = yosys_get_design();
			याव (design->selection_राशिः.size() > 1)
				design->निष्कासय_selection();
			log_reset_राशिः();
		}
		Tcl_SetResult(interp, (वर्ण *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		पटिनिवत्त TCL_ERROR;
	} गृहाण (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	सचे (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		सचे (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} अञ्ञथा
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} अञ्ञथा सचे ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	पटिनिवत्त TCL_OK;
}

#define FLAG(name) \
	सचे (!strcmp(argv[i], "-" #name)) { \
		name##_flag = सच्च; \
		अनुवर्त; \
	} \

#define FLAG2(name) \
	सचे (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = सच्च; \
		अनुवर्त; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (वर्ण *)(str), TCL_STATIC); \
		पटिनिवत्त TCL_ERROR; \
	}

तार्किक const_to_mp_int(ध्रुव Const &a, mp_int *b, तार्किक force_signed, तार्किक force_unsigned)
{
	सचे (!a.is_fully_def())
		पटिनिवत्त मिच्छा;

	सचे (mp_init(b))
		पटिनिवत्त मिच्छा;

	तार्किक negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	अत्थाय (पूर्णाङ्क i = a.size() - 1; i >= 0; i--) {
		सचे (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			पटिनिवत्त मिच्छा;
		}

		सचे ((a[i] == RTLIL::S1) ^ negative) {
			सचे (mp_add_d(b, 1, b)) {
				mp_clear(b);
				पटिनिवत्त मिच्छा;
			}
		}
	}

	सचे (negative) {
		सचे (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			पटिनिवत्त मिच्छा;
		}
	}

	पटिनिवत्त सच्च;
}

तार्किक mp_int_to_const(mp_int *a, Const &b, तार्किक is_signed)
{
	तार्किक negative = (mp_cmp_d(a, 0) == MP_LT);
	सचे (negative && !is_signed)
		पटिनिवत्त मिच्छा;

	सचे (negative) {
		सचे (mp_neg(a, a) != MP_OKAY)
			पटिनिवत्त मिच्छा;
		सचे (mp_sub_d(a, 1, a) != MP_OKAY)
			पटिनिवत्त मिच्छा;
	}

	std::vector<unsigned वर्ण> buf;
	buf.resize(mp_unsigned_bin_size(a));
	सचे (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		पटिनिवत्त मिच्छा;

	Const::Builder b_bits(mp_गणनम्_bits(a) + is_signed);
	अत्थाय (पूर्णाङ्क i = 0; i < mp_गणनम्_bits(a);) {
		अत्थाय (पूर्णाङ्क j = 0; j < 8 && i < mp_गणनम्_bits(a); j++, i++) {
			तार्किक bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.प्रक्षिप_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.निष्कासय_back();
	}

	सचे (is_signed) {
		b_bits.प्रक्षिप_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	पटिनिवत्त सच्च;
}

स्थिर पूर्णाङ्क tcl_get_attr(ClientData, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	पूर्णाङ्क i;
	तार्किक mod_flag = मिच्छा, string_flag = मिच्छा, bool_flag = मिच्छा;
	तार्किक int_flag = मिच्छा, sint_flag = मिच्छा, uint_flag = मिच्छा;
	अत्थाय (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(पूर्णाङ्क)
		FLAG(sint)
		FLAG(uint)
		FLAG(तार्किक)
		भङ्ग;
	}

	सचे ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	सचे (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	सचे (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	सचे (mod_flag) {
		obj = mod;
	} अञ्ञथा {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		सचे (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		सचे (!obj)
			obj = mod->cell(obj_twine);
		सचे (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	सचे (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	सचे (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} अञ्ञथा सचे (int_flag || uint_flag || sint_flag) {
		सचे (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &मानम् = obj->attributes.at(attr_twine);

		mp_int मानम्_mp;
		सचे (!const_to_mp_int(मानम्, &मानम्_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&मानम्_mp));
	} अञ्ञथा सचे (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} अञ्ञथा {
		सचे (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	पटिनिवत्त TCL_OK;
}

स्थिर पूर्णाङ्क tcl_has_attr(ClientData, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	पूर्णाङ्क i;
	तार्किक mod_flag = मिच्छा;
	अत्थाय (i = 1; i < argc; i++) {
		FLAG(mod)
		भङ्ग;
	}

	सचे ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	सचे (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	सचे (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	सचे (mod_flag) {
		obj = mod;
	} अञ्ञथा {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		सचे (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		सचे (!obj)
			obj = mod->cell(obj_twine);
		सचे (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	सचे (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	पटिनिवत्त TCL_OK;
}

स्थिर पूर्णाङ्क tcl_set_attr(ClientData, Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *ध्रुव objv[])
{
	पूर्णाङ्क i;
	तार्किक mod_flag = मिच्छा, string_flag = मिच्छा, bool_flag = मिच्छा;
	तार्किक true_flag = मिच्छा, false_flag = मिच्छा, sint_flag = मिच्छा, uint_flag = मिच्छा;
	अत्थाय (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(सच्च)
		FLAG2(मिच्छा)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(तार्किक)
		भङ्ग;
	}

	सचे ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	सचे (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	सचे (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	सचे (mod_flag) {
		obj = mod;
	} अञ्ञथा {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		सचे (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		सचे (!obj)
			obj = mod->cell(obj_twine);
		सचे (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	सचे (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	सचे (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} अञ्ञथा सचे (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int मानम्_mp;

		सचे (Tcl_TakeBignumFromObj(interp, objv[i++], &मानम्_mp))
			ERROR("non-integral value")

		सचे (!mp_int_to_const(&मानम्_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		सचे (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			सचे (const_.size() < 32)
				const_.exts(32);
		} अञ्ञथा {
			सचे (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} अञ्ञथा सचे (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} अञ्ञथा सचे (true_flag) {
		obj->set_bool_attribute(attr_twine, सच्च);
	} अञ्ञथा सचे (false_flag) {
		obj->set_bool_attribute(attr_twine, मिच्छा);
	} अञ्ञथा {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	पटिनिवत्त TCL_OK;
}

स्थिर पूर्णाङ्क tcl_get_param(ClientData, Tcl_Interp *interp, पूर्णाङ्क argc, ध्रुव वर्ण *argv[])
{
	पूर्णाङ्क i;
	तार्किक string_flag = मिच्छा;
	तार्किक int_flag = मिच्छा, sint_flag = मिच्छा, uint_flag = मिच्छा;
	अत्थाय (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(पूर्णाङ्क)
		FLAG(sint)
		FLAG(uint)
		भङ्ग;
	}

	सचे ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	सचे (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	सचे (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	सचे (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ध्रुव RTLIL::Const &मानम् = cell->getParam(param_twine);

	सचे (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(मानम्.decode_string().c_str(), -1));
	} अञ्ञथा सचे (int_flag || uint_flag || sint_flag) {
		mp_int मानम्_mp;
		सचे (!const_to_mp_int(मानम्, &मानम्_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&मानम्_mp));
	} अञ्ञथा {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(मानम्.as_string().c_str(), -1));
	}
	पटिनिवत्त TCL_OK;
}

स्थिर पूर्णाङ्क tcl_set_param(ClientData, Tcl_Interp *interp, पूर्णाङ्क objc, Tcl_Obj *ध्रुव objv[])
{
	पूर्णाङ्क i;
	तार्किक string_flag = मिच्छा, sint_flag = मिच्छा, uint_flag = मिच्छा;
	अत्थाय (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		भङ्ग;
	}

	सचे ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	सचे (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	सचे (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	सचे (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} अञ्ञथा सचे (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int मानम्_mp;

		सचे (Tcl_TakeBignumFromObj(interp, objv[i++], &मानम्_mp))
			ERROR("non-integral value")

		सचे (!mp_int_to_const(&मानम्_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		सचे (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			सचे (const_.size() < 32)
				const_.exts(32);
		} अञ्ञथा {
			सचे (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} अञ्ञथा {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	पटिनिवत्त TCL_OK;
}

पूर्णाङ्क yosys_tcl_interp_init(Tcl_Interp *interp)
{
	सचे (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_सन्देशः(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, रिक्त, रिक्त);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, रिक्त, रिक्त);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, रिक्त, रिक्त);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, रिक्त, रिक्त);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, रिक्त, रिक्त);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, रिक्त, रिक्त);

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

	पटिनिवत्त TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
