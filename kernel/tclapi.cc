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

#ifdef YOSYS_ENABLE_TCL
#شامل <tcl.h>
#شامل <tclTomMath.h>
#شامل <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
منطقی yosys_tcl_repl_active = نادرست;

تهی yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = درست;
}

ایستا Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ثابت json11::Json &json)
{
	اگر (json.is_null())
		بازگشت Tcl_NewStringObj("null", 4);
	وگرنه اگر (json.is_string()) {
		auto string = json.string_مقدار();
		بازگشت Tcl_NewStringObj(string.data(), string.size());
	} وگرنه اگر (json.is_عدد()) {
		دوگانه مقدار = json.عدد_مقدار();
		دوگانه round_val = std::nearbyint(مقدار);
		اگر (std::isfinite(round_val) && مقدار == round_val && مقدار >= LONG_MIN && مقدار < -دوگانه(LONG_MIN))
			بازگشت Tcl_NewLongObj((long)round_val);
		وگرنه
			بازگشت Tcl_NewDoubleObj(مقدار);
	} وگرنه اگر (json.is_bool()) {
		بازگشت Tcl_NewBooleanObj(json.bool_مقدار());
	} وگرنه اگر (json.is_array()) {
		auto list = json.array_اقلام();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		برای (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		بازگشت result;
	} وگرنه اگر (json.is_object()) {
		auto map = json.object_اقلام();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		برای (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		بازگشت result;
	} وگرنه {
		log_abort();
	}
}

ایستا صحیح tcl_yosys_cmd(ClientData, Tcl_Interp *interp, صحیح argc, ثابت نویسه *argv[])
{
	std::vector<std::string> args;
	برای (صحیح i = 1; i < argc; i++)
		args.افزودن_back(argv[i]);

	اگر (args.size() >= 1 && args[0] == "-import") {
		برای (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			اگر (tcl_command_name == "proc")
				tcl_command_name = "procs";
			وگرنه اگر (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			اگر (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} وگرنه {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		بازگشت TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	منطقی in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	تلاش {
		اگر (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} وگرنه {
			Pass::call(yosys_get_design(), args);
		}
	} بگیر (log_cmd_error_exception) {
		اگر (in_repl) {
			auto design = yosys_get_design();
			وقتی (design->selection_پشته.size() > 1)
				design->برداشت_selection();
			log_reset_پشته();
		}
		Tcl_SetResult(interp, (نویسه *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		بازگشت TCL_ERROR;
	} بگیر (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	اگر (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		اگر (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} وگرنه
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} وگرنه اگر ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	بازگشت TCL_OK;
}

#define FLAG(name) \
	اگر (!strcmp(argv[i], "-" #name)) { \
		name##_flag = درست; \
		ادامه; \
	} \

#define FLAG2(name) \
	اگر (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = درست; \
		ادامه; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (نویسه *)(str), TCL_STATIC); \
		بازگشت TCL_ERROR; \
	}

منطقی const_to_mp_int(ثابت Const &a, mp_int *b, منطقی force_signed, منطقی force_unsigned)
{
	اگر (!a.is_fully_def())
		بازگشت نادرست;

	اگر (mp_init(b))
		بازگشت نادرست;

	منطقی negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	برای (صحیح i = a.size() - 1; i >= 0; i--) {
		اگر (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			بازگشت نادرست;
		}

		اگر ((a[i] == RTLIL::S1) ^ negative) {
			اگر (mp_add_d(b, 1, b)) {
				mp_clear(b);
				بازگشت نادرست;
			}
		}
	}

	اگر (negative) {
		اگر (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			بازگشت نادرست;
		}
	}

	بازگشت درست;
}

منطقی mp_int_to_const(mp_int *a, Const &b, منطقی is_signed)
{
	منطقی negative = (mp_cmp_d(a, 0) == MP_LT);
	اگر (negative && !is_signed)
		بازگشت نادرست;

	اگر (negative) {
		اگر (mp_neg(a, a) != MP_OKAY)
			بازگشت نادرست;
		اگر (mp_sub_d(a, 1, a) != MP_OKAY)
			بازگشت نادرست;
	}

	std::vector<unsigned نویسه> buf;
	buf.resize(mp_unsigned_bin_size(a));
	اگر (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		بازگشت نادرست;

	Const::Builder b_bits(mp_شمار_bits(a) + is_signed);
	برای (صحیح i = 0; i < mp_شمار_bits(a);) {
		برای (صحیح j = 0; j < 8 && i < mp_شمار_bits(a); j++, i++) {
			منطقی bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.افزودن_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.برداشت_back();
	}

	اگر (is_signed) {
		b_bits.افزودن_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	بازگشت درست;
}

ایستا صحیح tcl_get_attr(ClientData, Tcl_Interp *interp, صحیح argc, ثابت نویسه *argv[])
{
	صحیح i;
	منطقی mod_flag = نادرست, string_flag = نادرست, bool_flag = نادرست;
	منطقی int_flag = نادرست, sint_flag = نادرست, uint_flag = نادرست;
	برای (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(صحیح)
		FLAG(sint)
		FLAG(uint)
		FLAG(منطقی)
		بشکن;
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	اگر (mod_flag) {
		obj = mod;
	} وگرنه {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		اگر (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		اگر (!obj)
			obj = mod->cell(obj_twine);
		اگر (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	اگر (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	اگر (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} وگرنه اگر (int_flag || uint_flag || sint_flag) {
		اگر (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &مقدار = obj->attributes.at(attr_twine);

		mp_int مقدار_mp;
		اگر (!const_to_mp_int(مقدار, &مقدار_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&مقدار_mp));
	} وگرنه اگر (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} وگرنه {
		اگر (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	بازگشت TCL_OK;
}

ایستا صحیح tcl_has_attr(ClientData, Tcl_Interp *interp, صحیح argc, ثابت نویسه *argv[])
{
	صحیح i;
	منطقی mod_flag = نادرست;
	برای (i = 1; i < argc; i++) {
		FLAG(mod)
		بشکن;
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	اگر (mod_flag) {
		obj = mod;
	} وگرنه {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		اگر (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		اگر (!obj)
			obj = mod->cell(obj_twine);
		اگر (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	اگر (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	بازگشت TCL_OK;
}

ایستا صحیح tcl_set_attr(ClientData, Tcl_Interp *interp, صحیح objc, Tcl_Obj *ثابت objv[])
{
	صحیح i;
	منطقی mod_flag = نادرست, string_flag = نادرست, bool_flag = نادرست;
	منطقی true_flag = نادرست, false_flag = نادرست, sint_flag = نادرست, uint_flag = نادرست;
	برای (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(درست)
		FLAG2(نادرست)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(منطقی)
		بشکن;
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

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	اگر (mod_flag) {
		obj = mod;
	} وگرنه {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		اگر (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		اگر (!obj)
			obj = mod->cell(obj_twine);
		اگر (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	اگر (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	اگر (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} وگرنه اگر (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int مقدار_mp;

		اگر (Tcl_TakeBignumFromObj(interp, objv[i++], &مقدار_mp))
			ERROR("non-integral value")

		اگر (!mp_int_to_const(&مقدار_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		اگر (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			اگر (const_.size() < 32)
				const_.exts(32);
		} وگرنه {
			اگر (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} وگرنه اگر (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} وگرنه اگر (true_flag) {
		obj->set_bool_attribute(attr_twine, درست);
	} وگرنه اگر (false_flag) {
		obj->set_bool_attribute(attr_twine, نادرست);
	} وگرنه {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	بازگشت TCL_OK;
}

ایستا صحیح tcl_get_param(ClientData, Tcl_Interp *interp, صحیح argc, ثابت نویسه *argv[])
{
	صحیح i;
	منطقی string_flag = نادرست;
	منطقی int_flag = نادرست, sint_flag = نادرست, uint_flag = نادرست;
	برای (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(صحیح)
		FLAG(sint)
		FLAG(uint)
		بشکن;
	}

	اگر ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	اگر (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	اگر (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ثابت RTLIL::Const &مقدار = cell->getParam(param_twine);

	اگر (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(مقدار.decode_string().c_str(), -1));
	} وگرنه اگر (int_flag || uint_flag || sint_flag) {
		mp_int مقدار_mp;
		اگر (!const_to_mp_int(مقدار, &مقدار_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&مقدار_mp));
	} وگرنه {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(مقدار.as_string().c_str(), -1));
	}
	بازگشت TCL_OK;
}

ایستا صحیح tcl_set_param(ClientData, Tcl_Interp *interp, صحیح objc, Tcl_Obj *ثابت objv[])
{
	صحیح i;
	منطقی string_flag = نادرست, sint_flag = نادرست, uint_flag = نادرست;
	برای (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		بشکن;
	}

	اگر ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	اگر (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	اگر (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	اگر (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} وگرنه اگر (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int مقدار_mp;

		اگر (Tcl_TakeBignumFromObj(interp, objv[i++], &مقدار_mp))
			ERROR("non-integral value")

		اگر (!mp_int_to_const(&مقدار_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		اگر (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			اگر (const_.size() < 32)
				const_.exts(32);
		} وگرنه {
			اگر (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} وگرنه {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	بازگشت TCL_OK;
}

صحیح yosys_tcl_interp_init(Tcl_Interp *interp)
{
	اگر (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_پیام(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, پوچ, پوچ);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, پوچ, پوچ);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, پوچ, پوچ);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, پوچ, پوچ);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, پوچ, پوچ);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, پوچ, پوچ);

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

	بازگشت TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
