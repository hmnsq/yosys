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

#समावेश "kernel/yosys.h"
#समावेश "kernel/rtlil.h"
#समावेश "libs/json11/json11.hpp"

#अगर_घोषित YOSYS_ENABLE_TCL
#समावेश <tcl.h>
#समावेश <tclTomMath.h>
#समावेश <tclTomMathDecls.h>
#पूर्ण_अगर

YOSYS_NAMESPACE_BEGIN

#अगर_घोषित YOSYS_ENABLE_TCL
bool yosys_tcl_repl_active = false;

व्योम yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = true;
}

अटल Tcl_Obj *json_to_tcl(Tcl_Interp *interp, स्थिर json11::Json &json)
{
	अगर (json.is_null())
		वापस Tcl_NewStringObj("null", 4);
	अन्यथा अगर (json.is_string()) {
		स्वतः string = json.string_मूल्य();
		वापस Tcl_NewStringObj(string.तथ्य(), string.आकार());
	} अन्यथा अगर (json.is_संख्या()) {
		द्विगुन मूल्य = json.संख्या_मूल्य();
		द्विगुन round_val = std::nearbyint(मूल्य);
		अगर (std::है_सीमित(round_val) && मूल्य == round_val && मूल्य >= दीर्घ_न्यून && मूल्य < -द्विगुन(दीर्घ_न्यून))
			वापस Tcl_NewLongObj((दीर्घ)round_val);
		अन्यथा
			वापस Tcl_NewDoubleObj(मूल्य);
	} अन्यथा अगर (json.is_bool()) {
		वापस Tcl_NewBooleanObj(json.bool_मूल्य());
	} अन्यथा अगर (json.is_array()) {
		स्वतः list = json.array_वस्तुएँ();
		Tcl_Obj *परिणाम = Tcl_NewListObj(list.आकार(), nullptr);
		क्रम (स्वतः &item : list)
			Tcl_ListObjAppendElement(interp, परिणाम, json_to_tcl(interp, item));
		वापस परिणाम;
	} अन्यथा अगर (json.is_object()) {
		स्वतः map = json.object_वस्तुएँ();
		Tcl_Obj *परिणाम = Tcl_NewListObj(map.आकार() * 2, nullptr);
		क्रम (स्वतः &item : map) {
			Tcl_ListObjAppendElement(interp, परिणाम, Tcl_NewStringObj(item.first.तथ्य(), item.first.आकार()));
			Tcl_ListObjAppendElement(interp, परिणाम, json_to_tcl(interp, item.second));
		}
		वापस परिणाम;
	} अन्यथा {
		log_abort();
	}
}

अटल पूर्णांक tcl_yosys_cmd(Client_तथ्य, Tcl_Interp *interp, पूर्णांक argc, स्थिर अक्षर *argv[])
{
	std::vector<std::string> args;
	क्रम (पूर्णांक i = 1; i < argc; i++)
		args.धकेलो_back(argv[i]);

	अगर (args.आकार() >= 1 && args[0] == "-import") {
		क्रम (स्वतः &it : pass_register) {
			std::string tcl_command_name = it.first;
			अगर (tcl_command_name == "proc")
				tcl_command_name = "procs";
			अन्यथा अगर (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			अगर (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} अन्यथा {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		वापस TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	bool in_repl = yosys_tcl_repl_active;
	स्वतः guard = logger().error_throw_scope();
	प्रयास {
		अगर (args.आकार() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} अन्यथा {
			Pass::call(yosys_get_design(), args);
		}
	} पकड़ो (log_cmd_error_exception) {
		अगर (in_repl) {
			स्वतः design = yosys_get_design();
			जबतक (design->selection_ढेर.आकार() > 1)
				design->निकालो_selection();
			log_reset_ढेर();
		}
		Tcl_Set_परिणाम(interp, (अक्षर *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		वापस TCL_ERROR;
	} पकड़ो (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	स्वतः &scratchpad = yosys_get_design()->scratchpad;
	स्वतः परिणाम = scratchpad.find("result.json");
	अगर (परिणाम != scratchpad.end()) {
		std::string err;
		स्वतः json = json11::Json::parse(परिणाम->second, err);
		अगर (err.empty()) {
			Tcl_Set_Obj_परिणाम(interp, json_to_tcl(interp, json));
		} अन्यथा
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} अन्यथा अगर ((परिणाम = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewStringObj(परिणाम->second.तथ्य(), परिणाम->second.आकार()));
	}

	वापस TCL_OK;
}

#घोषणा FLAG(name) \
	अगर (!म_भेद(argv[i], "-" #name)) { \
		name##_flag = true; \
		जारी; \
	} \

#घोषणा FLAG2(name) \
	अगर (!म_भेद(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = true; \
		जारी; \
	} \

#घोषणा ERROR(str) \
	{ \
		Tcl_Set_परिणाम(interp, (अक्षर *)(str), TCL_STATIC); \
		वापस TCL_ERROR; \
	}

bool const_to_mp_int(स्थिर Const &a, mp_int *b, bool force_signed, bool force_unsigned)
{
	अगर (!a.is_fully_def())
		वापस false;

	अगर (mp_init(b))
		वापस false;

	bool negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	क्रम (पूर्णांक i = a.आकार() - 1; i >= 0; i--) {
		अगर (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			वापस false;
		}

		अगर ((a[i] == RTLIL::S1) ^ negative) {
			अगर (mp_add_d(b, 1, b)) {
				mp_clear(b);
				वापस false;
			}
		}
	}

	अगर (negative) {
		अगर (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			वापस false;
		}
	}

	वापस true;
}

bool mp_int_to_const(mp_int *a, Const &b, bool is_signed)
{
	bool negative = (mp_cmp_d(a, 0) == MP_LT);
	अगर (negative && !is_signed)
		वापस false;

	अगर (negative) {
		अगर (mp_neg(a, a) != MP_OKAY)
			वापस false;
		अगर (mp_sub_d(a, 1, a) != MP_OKAY)
			वापस false;
	}

	std::vector<अचिन्हित अक्षर> buf;
	buf.resize(mp_unsigned_bin_आकार(a));
	अगर (mp_to_unsigned_bin(a, buf.तथ्य()) != MP_OKAY)
		वापस false;

	Const::Builder b_bits(mp_गिनती_bits(a) + is_signed);
	क्रम (पूर्णांक i = 0; i < mp_गिनती_bits(a);) {
		क्रम (पूर्णांक j = 0; j < 8 && i < mp_गिनती_bits(a); j++, i++) {
			bool bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.धकेलो_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.निकालो_back();
	}

	अगर (is_signed) {
		b_bits.धकेलो_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	वापस true;
}

अटल पूर्णांक tcl_get_attr(Client_तथ्य, Tcl_Interp *interp, पूर्णांक argc, स्थिर अक्षर *argv[])
{
	पूर्णांक i;
	bool mod_flag = false, string_flag = false, bool_flag = false;
	bool int_flag = false, sint_flag = false, uint_flag = false;
	क्रम (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(पूर्णांक)
		FLAG(sint)
		FLAG(uint)
		FLAG(bool)
		अवरोध;
	}

	अगर ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	अगर (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	स्वतः mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	अगर (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	अगर (mod_flag) {
		obj = mod;
	} अन्यथा {
		स्वतः obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		अगर (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		अगर (!obj)
			obj = mod->cell(obj_twine);
		अगर (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	अगर (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	अगर (string_flag) {
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} अन्यथा अगर (int_flag || uint_flag || sint_flag) {
		अगर (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &मूल्य = obj->attributes.at(attr_twine);

		mp_int मूल्य_mp;
		अगर (!const_to_mp_int(मूल्य, &मूल्य_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewBignumObj(&मूल्य_mp));
	} अन्यथा अगर (bool_flag) {
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} अन्यथा {
		अगर (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_Set_Obj_परिणाम(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	वापस TCL_OK;
}

अटल पूर्णांक tcl_has_attr(Client_तथ्य, Tcl_Interp *interp, पूर्णांक argc, स्थिर अक्षर *argv[])
{
	पूर्णांक i;
	bool mod_flag = false;
	क्रम (i = 1; i < argc; i++) {
		FLAG(mod)
		अवरोध;
	}

	अगर ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	अगर (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	स्वतः mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	अगर (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	अगर (mod_flag) {
		obj = mod;
	} अन्यथा {
		स्वतः obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		अगर (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		अगर (!obj)
			obj = mod->cell(obj_twine);
		अगर (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	अगर (!obj)
		ERROR("object not found")

	Tcl_Set_Obj_परिणाम(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	वापस TCL_OK;
}

अटल पूर्णांक tcl_set_attr(Client_तथ्य, Tcl_Interp *interp, पूर्णांक objc, Tcl_Obj *स्थिर objv[])
{
	पूर्णांक i;
	bool mod_flag = false, string_flag = false, bool_flag = false;
	bool true_flag = false, false_flag = false, sint_flag = false, uint_flag = false;
	क्रम (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(true)
		FLAG2(false)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(bool)
		अवरोध;
	}

	अगर ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	अगर (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	स्वतः mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	अगर (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	अगर (mod_flag) {
		obj = mod;
	} अन्यथा {
		स्वतः obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		अगर (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		अगर (!obj)
			obj = mod->cell(obj_twine);
		अगर (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	अगर (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	अगर (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} अन्यथा अगर (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int मूल्य_mp;

		अगर (Tcl_TakeBignumFromObj(interp, objv[i++], &मूल्य_mp))
			ERROR("non-integral value")

		अगर (!mp_int_to_const(&मूल्य_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		अगर (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			अगर (const_.आकार() < 32)
				const_.exts(32);
		} अन्यथा {
			अगर (const_.आकार() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} अन्यथा अगर (bool_flag) {
		obj->set_bool_attribute(attr_twine, म_से_प(Tcl_GetString(objv[i++])) != 0);
	} अन्यथा अगर (true_flag) {
		obj->set_bool_attribute(attr_twine, true);
	} अन्यथा अगर (false_flag) {
		obj->set_bool_attribute(attr_twine, false);
	} अन्यथा {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	वापस TCL_OK;
}

अटल पूर्णांक tcl_get_param(Client_तथ्य, Tcl_Interp *interp, पूर्णांक argc, स्थिर अक्षर *argv[])
{
	पूर्णांक i;
	bool string_flag = false;
	bool int_flag = false, sint_flag = false, uint_flag = false;
	क्रम (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(पूर्णांक)
		FLAG(sint)
		FLAG(uint)
		अवरोध;
	}

	अगर ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	स्वतः mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	अगर (!mod)
		ERROR("module not found")

	स्वतः cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	अगर (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	अगर (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	स्थिर RTLIL::Const &मूल्य = cell->getParam(param_twine);

	अगर (string_flag) {
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewStringObj(मूल्य.decode_string().c_str(), -1));
	} अन्यथा अगर (int_flag || uint_flag || sint_flag) {
		mp_int मूल्य_mp;
		अगर (!const_to_mp_int(मूल्य, &मूल्य_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewBignumObj(&मूल्य_mp));
	} अन्यथा {
		Tcl_Set_Obj_परिणाम(interp, Tcl_NewStringObj(मूल्य.as_string().c_str(), -1));
	}
	वापस TCL_OK;
}

अटल पूर्णांक tcl_set_param(Client_तथ्य, Tcl_Interp *interp, पूर्णांक objc, Tcl_Obj *स्थिर objv[])
{
	पूर्णांक i;
	bool string_flag = false, sint_flag = false, uint_flag = false;
	क्रम (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		अवरोध;
	}

	अगर ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	स्वतः mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	अगर (!mod)
		ERROR("module not found")

	स्वतः cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	अगर (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	अगर (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} अन्यथा अगर (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int मूल्य_mp;

		अगर (Tcl_TakeBignumFromObj(interp, objv[i++], &मूल्य_mp))
			ERROR("non-integral value")

		अगर (!mp_int_to_const(&मूल्य_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		अगर (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			अगर (const_.आकार() < 32)
				const_.exts(32);
		} अन्यथा {
			अगर (const_.आकार() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} अन्यथा {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	वापस TCL_OK;
}

पूर्णांक yosys_tcl_interp_init(Tcl_Interp *interp)
{
	अगर (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_संदेश(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, शून्य, शून्य);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, शून्य, शून्य);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, शून्य, शून्य);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, शून्य, शून्य);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, शून्य, शून्य);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, शून्य, शून्य);

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

	वापस TCL_OK ;
}
#पूर्ण_अगर

YOSYS_NAMESPACE_END
