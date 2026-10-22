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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#অন্তর্ভাব <tcl.h>
#অন্তর্ভাব <tclTomMath.h>
#অন্তর্ভাব <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
যৌক্তিক yosys_tcl_repl_active = মিথ্যা;

শূন্য yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = সত্য;
}

স্থির Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ধ্রুবক json11::Json &json)
{
	যদি (json.is_null())
		ফেরত Tcl_NewStringObj("null", 4);
	অন্যথা যদি (json.is_string()) {
		auto string = json.string_মান();
		ফেরত Tcl_NewStringObj(string.তথ্য(), string.আকার_2());
	} অন্যথা যদি (json.is_সংখ্যা()) {
		দ্বিগুণ মান = json.সংখ্যা_মান();
		দ্বিগুণ round_val = std::nearbyint(মান);
		যদি (std::isfinite(round_val) && মান == round_val && মান >= LONG_MIN && মান < -দ্বিগুণ(LONG_MIN))
			ফেরত Tcl_NewLongObj((long)round_val);
		অন্যথা
			ফেরত Tcl_NewDoubleObj(মান);
	} অন্যথা যদি (json.is_bool()) {
		ফেরত Tcl_NewBooleanObj(json.bool_মান());
	} অন্যথা যদি (json.is_array()) {
		auto list = json.array_বস্তুসমূহ();
		Tcl_Obj *ফলাফল = Tcl_NewListObj(list.আকার_2(), nullptr);
		জন্য (auto &item : list)
			Tcl_ListObjAppendElement(interp, ফলাফল, json_to_tcl(interp, item));
		ফেরত ফলাফল;
	} অন্যথা যদি (json.is_object()) {
		auto map = json.object_বস্তুসমূহ();
		Tcl_Obj *ফলাফল = Tcl_NewListObj(map.আকার_2() * 2, nullptr);
		জন্য (auto &item : map) {
			Tcl_ListObjAppendElement(interp, ফলাফল, Tcl_NewStringObj(item.first.তথ্য(), item.first.আকার_2()));
			Tcl_ListObjAppendElement(interp, ফলাফল, json_to_tcl(interp, item.second));
		}
		ফেরত ফলাফল;
	} অন্যথা {
		log_abort();
	}
}

স্থির পূর্ণসংখ্যা tcl_yosys_cmd(Client_তথ্য, Tcl_Interp *interp, পূর্ণসংখ্যা argc, ধ্রুবক অক্ষর *argv[])
{
	std::vector<std::string> args;
	জন্য (পূর্ণসংখ্যা i = 1; i < argc; i++)
		args.ঠেলো_back(argv[i]);

	যদি (args.আকার_2() >= 1 && args[0] == "-import") {
		জন্য (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			যদি (tcl_command_name == "proc")
				tcl_command_name = "procs";
			অন্যথা যদি (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			যদি (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} অন্যথা {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ফেরত TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	যৌক্তিক in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	চেষ্টা {
		যদি (args.আকার_2() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} অন্যথা {
			Pass::call(yosys_get_design(), args);
		}
	} ধরো (log_cmd_error_exception) {
		যদি (in_repl) {
			auto design = yosys_get_design();
			যখন (design->selection_স্তূপ.আকার_2() > 1)
				design->বের_করো_selection();
			log_reset_স্তূপ();
		}
		Tcl_Set_ফলাফল(interp, (অক্ষর *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		ফেরত TCL_ERROR;
	} ধরো (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto ফলাফল = scratchpad.find("result.json");
	যদি (ফলাফল != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(ফলাফল->second, err);
		যদি (err.empty()) {
			Tcl_Set_Obj_ফলাফল(interp, json_to_tcl(interp, json));
		} অন্যথা
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} অন্যথা যদি ((ফলাফল = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewStringObj(ফলাফল->second.তথ্য(), ফলাফল->second.আকার_2()));
	}

	ফেরত TCL_OK;
}

#define FLAG(name) \
	যদি (!strcmp(argv[i], "-" #name)) { \
		name##_flag = সত্য; \
		চালু; \
	} \

#define FLAG2(name) \
	যদি (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = সত্য; \
		চালু; \
	} \

#define ERROR(str) \
	{ \
		Tcl_Set_ফলাফল(interp, (অক্ষর *)(str), TCL_STATIC); \
		ফেরত TCL_ERROR; \
	}

যৌক্তিক const_to_mp_int(ধ্রুবক Const &a, mp_int *b, যৌক্তিক force_signed, যৌক্তিক force_unsigned)
{
	যদি (!a.is_fully_def())
		ফেরত মিথ্যা;

	যদি (mp_init(b))
		ফেরত মিথ্যা;

	যৌক্তিক negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	জন্য (পূর্ণসংখ্যা i = a.আকার_2() - 1; i >= 0; i--) {
		যদি (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ফেরত মিথ্যা;
		}

		যদি ((a[i] == RTLIL::S1) ^ negative) {
			যদি (mp_add_d(b, 1, b)) {
				mp_clear(b);
				ফেরত মিথ্যা;
			}
		}
	}

	যদি (negative) {
		যদি (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ফেরত মিথ্যা;
		}
	}

	ফেরত সত্য;
}

যৌক্তিক mp_int_to_const(mp_int *a, Const &b, যৌক্তিক is_signed)
{
	যৌক্তিক negative = (mp_cmp_d(a, 0) == MP_LT);
	যদি (negative && !is_signed)
		ফেরত মিথ্যা;

	যদি (negative) {
		যদি (mp_neg(a, a) != MP_OKAY)
			ফেরত মিথ্যা;
		যদি (mp_sub_d(a, 1, a) != MP_OKAY)
			ফেরত মিথ্যা;
	}

	std::vector<unsigned অক্ষর> buf;
	buf.resize(mp_unsigned_bin_আকার(a));
	যদি (mp_to_unsigned_bin(a, buf.তথ্য()) != MP_OKAY)
		ফেরত মিথ্যা;

	Const::Builder b_bits(mp_গুনতি_bits(a) + is_signed);
	জন্য (পূর্ণসংখ্যা i = 0; i < mp_গুনতি_bits(a);) {
		জন্য (পূর্ণসংখ্যা j = 0; j < 8 && i < mp_গুনতি_bits(a); j++, i++) {
			যৌক্তিক bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.ঠেলো_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.বের_করো_back();
	}

	যদি (is_signed) {
		b_bits.ঠেলো_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	ফেরত সত্য;
}

স্থির পূর্ণসংখ্যা tcl_get_attr(Client_তথ্য, Tcl_Interp *interp, পূর্ণসংখ্যা argc, ধ্রুবক অক্ষর *argv[])
{
	পূর্ণসংখ্যা i;
	যৌক্তিক mod_flag = মিথ্যা, string_flag = মিথ্যা, bool_flag = মিথ্যা;
	যৌক্তিক int_flag = মিথ্যা, sint_flag = মিথ্যা, uint_flag = মিথ্যা;
	জন্য (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(পূর্ণসংখ্যা)
		FLAG(sint)
		FLAG(uint)
		FLAG(যৌক্তিক)
		বিরতি;
	}

	যদি ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	যদি (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	যদি (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	যদি (mod_flag) {
		obj = mod;
	} অন্যথা {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		যদি (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		যদি (!obj)
			obj = mod->cell(obj_twine);
		যদি (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	যদি (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	যদি (string_flag) {
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} অন্যথা যদি (int_flag || uint_flag || sint_flag) {
		যদি (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &মান = obj->attributes.at(attr_twine);

		mp_int মান_mp;
		যদি (!const_to_mp_int(মান, &মান_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewBignumObj(&মান_mp));
	} অন্যথা যদি (bool_flag) {
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} অন্যথা {
		যদি (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	ফেরত TCL_OK;
}

স্থির পূর্ণসংখ্যা tcl_has_attr(Client_তথ্য, Tcl_Interp *interp, পূর্ণসংখ্যা argc, ধ্রুবক অক্ষর *argv[])
{
	পূর্ণসংখ্যা i;
	যৌক্তিক mod_flag = মিথ্যা;
	জন্য (i = 1; i < argc; i++) {
		FLAG(mod)
		বিরতি;
	}

	যদি ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	যদি (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	যদি (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	যদি (mod_flag) {
		obj = mod;
	} অন্যথা {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		যদি (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		যদি (!obj)
			obj = mod->cell(obj_twine);
		যদি (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	যদি (!obj)
		ERROR("object not found")

	Tcl_Set_Obj_ফলাফল(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	ফেরত TCL_OK;
}

স্থির পূর্ণসংখ্যা tcl_set_attr(Client_তথ্য, Tcl_Interp *interp, পূর্ণসংখ্যা objc, Tcl_Obj *ধ্রুবক objv[])
{
	পূর্ণসংখ্যা i;
	যৌক্তিক mod_flag = মিথ্যা, string_flag = মিথ্যা, bool_flag = মিথ্যা;
	যৌক্তিক true_flag = মিথ্যা, false_flag = মিথ্যা, sint_flag = মিথ্যা, uint_flag = মিথ্যা;
	জন্য (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(সত্য)
		FLAG2(মিথ্যা)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(যৌক্তিক)
		বিরতি;
	}

	যদি ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	যদি (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	যদি (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	যদি (mod_flag) {
		obj = mod;
	} অন্যথা {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		যদি (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		যদি (!obj)
			obj = mod->cell(obj_twine);
		যদি (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	যদি (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	যদি (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} অন্যথা যদি (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int মান_mp;

		যদি (Tcl_TakeBignumFromObj(interp, objv[i++], &মান_mp))
			ERROR("non-integral value")

		যদি (!mp_int_to_const(&মান_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		যদি (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			যদি (const_.আকার_2() < 32)
				const_.exts(32);
		} অন্যথা {
			যদি (const_.আকার_2() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} অন্যথা যদি (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} অন্যথা যদি (true_flag) {
		obj->set_bool_attribute(attr_twine, সত্য);
	} অন্যথা যদি (false_flag) {
		obj->set_bool_attribute(attr_twine, মিথ্যা);
	} অন্যথা {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ফেরত TCL_OK;
}

স্থির পূর্ণসংখ্যা tcl_get_param(Client_তথ্য, Tcl_Interp *interp, পূর্ণসংখ্যা argc, ধ্রুবক অক্ষর *argv[])
{
	পূর্ণসংখ্যা i;
	যৌক্তিক string_flag = মিথ্যা;
	যৌক্তিক int_flag = মিথ্যা, sint_flag = মিথ্যা, uint_flag = মিথ্যা;
	জন্য (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(পূর্ণসংখ্যা)
		FLAG(sint)
		FLAG(uint)
		বিরতি;
	}

	যদি ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	যদি (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	যদি (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	যদি (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ধ্রুবক RTLIL::Const &মান = cell->getParam(param_twine);

	যদি (string_flag) {
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewStringObj(মান.decode_string().c_str(), -1));
	} অন্যথা যদি (int_flag || uint_flag || sint_flag) {
		mp_int মান_mp;
		যদি (!const_to_mp_int(মান, &মান_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewBignumObj(&মান_mp));
	} অন্যথা {
		Tcl_Set_Obj_ফলাফল(interp, Tcl_NewStringObj(মান.as_string().c_str(), -1));
	}
	ফেরত TCL_OK;
}

স্থির পূর্ণসংখ্যা tcl_set_param(Client_তথ্য, Tcl_Interp *interp, পূর্ণসংখ্যা objc, Tcl_Obj *ধ্রুবক objv[])
{
	পূর্ণসংখ্যা i;
	যৌক্তিক string_flag = মিথ্যা, sint_flag = মিথ্যা, uint_flag = মিথ্যা;
	জন্য (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		বিরতি;
	}

	যদি ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	যদি (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	যদি (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	যদি (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} অন্যথা যদি (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int মান_mp;

		যদি (Tcl_TakeBignumFromObj(interp, objv[i++], &মান_mp))
			ERROR("non-integral value")

		যদি (!mp_int_to_const(&মান_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		যদি (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			যদি (const_.আকার_2() < 32)
				const_.exts(32);
		} অন্যথা {
			যদি (const_.আকার_2() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} অন্যথা {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ফেরত TCL_OK;
}

পূর্ণসংখ্যা yosys_tcl_interp_init(Tcl_Interp *interp)
{
	যদি (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_বার্তা(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ফাঁকা, ফাঁকা);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ফাঁকা, ফাঁকা);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ফাঁকা, ফাঁকা);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, ফাঁকা, ফাঁকা);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ফাঁকা, ফাঁকা);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, ফাঁকা, ফাঁকা);

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

	ফেরত TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
