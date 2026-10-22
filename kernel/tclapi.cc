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

#ifdef YOSYS_ENABLE_TCL
#ضمن <tcl.h>
#ضمن <tclTomMath.h>
#ضمن <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
منطقي yosys_tcl_repl_active = خطأ;

فراغ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = true;
}

ساكن Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ثابت json11::Json &json)
{
	إذا (json.is_null())
		أرجع Tcl_NewStringObj("null", 4);
	وإلا إذا (json.is_string()) {
		auto string = json.string_قيمة();
		أرجع Tcl_NewStringObj(string.data(), string.size());
	} وإلا إذا (json.is_رقم()) {
		مزدوج قيمة = json.رقم_قيمة();
		مزدوج round_val = std::nearbyint(قيمة);
		إذا (std::isfinite(round_val) && قيمة == round_val && قيمة >= LONG_MIN && قيمة < -مزدوج(LONG_MIN))
			أرجع Tcl_NewLongObj((long)round_val);
		وإلا
			أرجع Tcl_NewDoubleObj(قيمة);
	} وإلا إذا (json.is_bool()) {
		أرجع Tcl_NewBooleanObj(json.bool_قيمة());
	} وإلا إذا (json.is_array()) {
		auto list = json.array_عناصر();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		لكل (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		أرجع result;
	} وإلا إذا (json.is_object()) {
		auto map = json.object_عناصر();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		لكل (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		أرجع result;
	} وإلا {
		log_abort();
	}
}

ساكن صحيح tcl_yosys_cmd(ClientData, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
{
	std::vector<std::string> وسائط_2;
	لكل (صحيح i = 1; i < argc; i++)
		وسائط_2.ادفع_back(argv[i]);

	إذا (وسائط_2.size() >= 1 && وسائط_2[0] == "-import") {
		لكل (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			إذا (tcl_command_name == "proc")
				tcl_command_name = "procs";
			وإلا إذا (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			إذا (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} وإلا {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		أرجع TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	منطقي in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	حاول {
		إذا (وسائط_2.size() == 1) {
			Pass::call(yosys_get_design(), وسائط_2[0]);
		} وإلا {
			Pass::call(yosys_get_design(), وسائط_2);
		}
	} امسك (log_cmd_error_exception) {
		إذا (in_repl) {
			auto design = yosys_get_design();
			طالما (design->selection_مكدس.size() > 1)
				design->اسحب_selection();
			log_reset_مكدس();
		}
		Tcl_SetResult(interp, (حرف *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		أرجع TCL_ERROR;
	} امسك (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	إذا (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		إذا (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} وإلا
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} وإلا إذا ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	أرجع TCL_OK;
}

#define FLAG(name) \
	إذا (!strcmp(argv[i], "-" #name)) { \
		name##_flag = true; \
		استمر; \
	} \

#define FLAG2(name) \
	إذا (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = true; \
		استمر; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (حرف *)(str), TCL_STATIC); \
		أرجع TCL_ERROR; \
	}

منطقي const_to_mp_int(ثابت Const &a, mp_int *b, منطقي force_signed, منطقي force_unsigned)
{
	إذا (!a.is_fully_def())
		أرجع خطأ;

	إذا (mp_init(b))
		أرجع خطأ;

	منطقي negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	لكل (صحيح i = a.size() - 1; i >= 0; i--) {
		إذا (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			أرجع خطأ;
		}

		إذا ((a[i] == RTLIL::S1) ^ negative) {
			إذا (mp_add_d(b, 1, b)) {
				mp_clear(b);
				أرجع خطأ;
			}
		}
	}

	إذا (negative) {
		إذا (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			أرجع خطأ;
		}
	}

	أرجع true;
}

منطقي mp_int_to_const(mp_int *a, Const &b, منطقي is_signed)
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

	std::vector<unsigned حرف> buf;
	buf.resize(mp_unsigned_bin_size(a));
	إذا (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		أرجع خطأ;

	Const::Builder b_bits(mp_عدد_bits(a) + is_signed);
	لكل (صحيح i = 0; i < mp_عدد_bits(a);) {
		لكل (صحيح j = 0; j < 8 && i < mp_عدد_bits(a); j++, i++) {
			منطقي bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.ادفع_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.اسحب_back();
	}

	إذا (is_signed) {
		b_bits.ادفع_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	أرجع true;
}

ساكن صحيح tcl_get_attr(ClientData, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	إذا (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &قيمة = obj->attributes.at(attr_twine);

		mp_int قيمة_mp;
		إذا (!const_to_mp_int(قيمة, &قيمة_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&قيمة_mp));
	} وإلا إذا (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} وإلا {
		إذا (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	أرجع TCL_OK;
}

ساكن صحيح tcl_has_attr(ClientData, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	إذا (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	أرجع TCL_OK;
}

ساكن صحيح tcl_set_attr(ClientData, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
{
	صحيح i;
	منطقي mod_flag = خطأ, string_flag = خطأ, bool_flag = خطأ;
	منطقي true_flag = خطأ, false_flag = خطأ, sint_flag = خطأ, uint_flag = خطأ;
	لكل (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(true)
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	إذا (mod_flag) {
		obj = mod;
	} وإلا {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		إذا (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		إذا (!obj)
			obj = mod->cell(obj_twine);
		إذا (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	إذا (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	إذا (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int قيمة_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &قيمة_mp))
			ERROR("non-integral value")

		إذا (!mp_int_to_const(&قيمة_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.size() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} وإلا إذا (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} وإلا إذا (true_flag) {
		obj->set_bool_attribute(attr_twine, true);
	} وإلا إذا (false_flag) {
		obj->set_bool_attribute(attr_twine, خطأ);
	} وإلا {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	أرجع TCL_OK;
}

ساكن صحيح tcl_get_param(ClientData, Tcl_Interp *interp, صحيح argc, ثابت حرف *argv[])
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	إذا (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ثابت RTLIL::Const &قيمة = cell->getParam(param_twine);

	إذا (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(قيمة.decode_string().c_str(), -1));
	} وإلا إذا (int_flag || uint_flag || sint_flag) {
		mp_int قيمة_mp;
		إذا (!const_to_mp_int(قيمة, &قيمة_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&قيمة_mp));
	} وإلا {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(قيمة.as_string().c_str(), -1));
	}
	أرجع TCL_OK;
}

ساكن صحيح tcl_set_param(ClientData, Tcl_Interp *interp, صحيح objc, Tcl_Obj *ثابت objv[])
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	إذا (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	إذا (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	إذا (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} وإلا إذا (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int قيمة_mp;

		إذا (Tcl_TakeBignumFromObj(interp, objv[i++], &قيمة_mp))
			ERROR("non-integral value")

		إذا (!mp_int_to_const(&قيمة_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		إذا (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			إذا (const_.size() < 32)
				const_.exts(32);
		} وإلا {
			إذا (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} وإلا {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	أرجع TCL_OK;
}

صحيح yosys_tcl_interp_init(Tcl_Interp *interp)
{
	إذا (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_رسالة(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, عدم, عدم);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, عدم, عدم);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, عدم, عدم);

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
#endif

YOSYS_NAMESPACE_END
