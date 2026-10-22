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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/rtlil.h"
#অন্তর্ভাৱ "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#অন্তর্ভাৱ <tcl.h>
#অন্তর্ভাৱ <tclTomMath.h>
#অন্তর্ভাৱ <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
তার্কিক yosys_tcl_repl_active = মিছা;

শূন্য yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = সঁচা;
}

স্থির Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ধ্রুৱ json11::Json &json)
{
	যদি (json.is_null())
		ঘূৰাই Tcl_NewStringObj("null", 4);
	অন্যথা যদি (json.is_string()) {
		auto string = json.string_মান();
		ঘূৰাই Tcl_NewStringObj(string.data(), string.size());
	} অন্যথা যদি (json.is_সংখ্যা()) {
		দ্ৱিগুণ মান = json.সংখ্যা_মান();
		দ্ৱিগুণ round_val = std::nearbyint(মান);
		যদি (std::isfinite(round_val) && মান == round_val && মান >= LONG_MIN && মান < -দ্ৱিগুণ(LONG_MIN))
			ঘূৰাই Tcl_NewLongObj((long)round_val);
		অন্যথা
			ঘূৰাই Tcl_NewDoubleObj(মান);
	} অন্যথা যদি (json.is_bool()) {
		ঘূৰাই Tcl_NewBooleanObj(json.bool_মান());
	} অন্যথা যদি (json.is_array()) {
		auto list = json.array_বস্তুসমূহ();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		বাবে (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		ঘূৰাই result;
	} অন্যথা যদি (json.is_object()) {
		auto map = json.object_বস্তুসমূহ();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		বাবে (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		ঘূৰাই result;
	} অন্যথা {
		log_abort();
	}
}

স্থির পূর্ণাঙ্ক tcl_yosys_cmd(ClientData, Tcl_Interp *interp, পূর্ণাঙ্ক argc, ধ্রুৱ ৱর্ণ *argv[])
{
	std::vector<std::string> args;
	বাবে (পূর্ণাঙ্ক i = 1; i < argc; i++)
		args.ঠেলো_back(argv[i]);

	যদি (args.size() >= 1 && args[0] == "-import") {
		বাবে (auto &it : pass_register) {
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
		ঘূৰাই TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	তার্কিক in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	যত্ন {
		যদি (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} অন্যথা {
			Pass::call(yosys_get_design(), args);
		}
	} গৃহাণ (log_cmd_error_exception) {
		যদি (in_repl) {
			auto design = yosys_get_design();
			যেতিয়ালৈ (design->selection_স্তূপ.size() > 1)
				design->বেৰ_কৰো_selection();
			log_reset_স্তূপ();
		}
		Tcl_SetResult(interp, (ৱর্ণ *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		ঘূৰাই TCL_ERROR;
	} গৃহাণ (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	যদি (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		যদি (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} অন্যথা
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} অন্যথা যদি ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	ঘূৰাই TCL_OK;
}

#define FLAG(name) \
	যদি (!strcmp(argv[i], "-" #name)) { \
		name##_flag = সঁচা; \
		অনুৱর্ত; \
	} \

#define FLAG2(name) \
	যদি (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = সঁচা; \
		অনুৱর্ত; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (ৱর্ণ *)(str), TCL_STATIC); \
		ঘূৰাই TCL_ERROR; \
	}

তার্কিক const_to_mp_int(ধ্রুৱ Const &a, mp_int *b, তার্কিক force_signed, তার্কিক force_unsigned)
{
	যদি (!a.is_fully_def())
		ঘূৰাই মিছা;

	যদি (mp_init(b))
		ঘূৰাই মিছা;

	তার্কিক negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	বাবে (পূর্ণাঙ্ক i = a.size() - 1; i >= 0; i--) {
		যদি (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ঘূৰাই মিছা;
		}

		যদি ((a[i] == RTLIL::S1) ^ negative) {
			যদি (mp_add_d(b, 1, b)) {
				mp_clear(b);
				ঘূৰাই মিছা;
			}
		}
	}

	যদি (negative) {
		যদি (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ঘূৰাই মিছা;
		}
	}

	ঘূৰাই সঁচা;
}

তার্কিক mp_int_to_const(mp_int *a, Const &b, তার্কিক is_signed)
{
	তার্কিক negative = (mp_cmp_d(a, 0) == MP_LT);
	যদি (negative && !is_signed)
		ঘূৰাই মিছা;

	যদি (negative) {
		যদি (mp_neg(a, a) != MP_OKAY)
			ঘূৰাই মিছা;
		যদি (mp_sub_d(a, 1, a) != MP_OKAY)
			ঘূৰাই মিছা;
	}

	std::vector<unsigned ৱর্ণ> buf;
	buf.resize(mp_unsigned_bin_size(a));
	যদি (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		ঘূৰাই মিছা;

	Const::Builder b_bits(mp_গুনতি_bits(a) + is_signed);
	বাবে (পূর্ণাঙ্ক i = 0; i < mp_গুনতি_bits(a);) {
		বাবে (পূর্ণাঙ্ক j = 0; j < 8 && i < mp_গুনতি_bits(a); j++, i++) {
			তার্কিক bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.ঠেলো_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.বেৰ_কৰো_back();
	}

	যদি (is_signed) {
		b_bits.ঠেলো_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	ঘূৰাই সঁচা;
}

স্থির পূর্ণাঙ্ক tcl_get_attr(ClientData, Tcl_Interp *interp, পূর্ণাঙ্ক argc, ধ্রুৱ ৱর্ণ *argv[])
{
	পূর্ণাঙ্ক i;
	তার্কিক mod_flag = মিছা, string_flag = মিছা, bool_flag = মিছা;
	তার্কিক int_flag = মিছা, sint_flag = মিছা, uint_flag = মিছা;
	বাবে (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(পূর্ণাঙ্ক)
		FLAG(sint)
		FLAG(uint)
		FLAG(তার্কিক)
		ভঙ্গ;
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
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} অন্যথা যদি (int_flag || uint_flag || sint_flag) {
		যদি (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &মান = obj->attributes.at(attr_twine);

		mp_int মান_mp;
		যদি (!const_to_mp_int(মান, &মান_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&মান_mp));
	} অন্যথা যদি (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} অন্যথা {
		যদি (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	ঘূৰাই TCL_OK;
}

স্থির পূর্ণাঙ্ক tcl_has_attr(ClientData, Tcl_Interp *interp, পূর্ণাঙ্ক argc, ধ্রুৱ ৱর্ণ *argv[])
{
	পূর্ণাঙ্ক i;
	তার্কিক mod_flag = মিছা;
	বাবে (i = 1; i < argc; i++) {
		FLAG(mod)
		ভঙ্গ;
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

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	ঘূৰাই TCL_OK;
}

স্থির পূর্ণাঙ্ক tcl_set_attr(ClientData, Tcl_Interp *interp, পূর্ণাঙ্ক objc, Tcl_Obj *ধ্রুৱ objv[])
{
	পূর্ণাঙ্ক i;
	তার্কিক mod_flag = মিছা, string_flag = মিছা, bool_flag = মিছা;
	তার্কিক true_flag = মিছা, false_flag = মিছা, sint_flag = মিছা, uint_flag = মিছা;
	বাবে (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(সঁচা)
		FLAG2(মিছা)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(তার্কিক)
		ভঙ্গ;
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
			যদি (const_.size() < 32)
				const_.exts(32);
		} অন্যথা {
			যদি (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} অন্যথা যদি (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} অন্যথা যদি (true_flag) {
		obj->set_bool_attribute(attr_twine, সঁচা);
	} অন্যথা যদি (false_flag) {
		obj->set_bool_attribute(attr_twine, মিছা);
	} অন্যথা {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ঘূৰাই TCL_OK;
}

স্থির পূর্ণাঙ্ক tcl_get_param(ClientData, Tcl_Interp *interp, পূর্ণাঙ্ক argc, ধ্রুৱ ৱর্ণ *argv[])
{
	পূর্ণাঙ্ক i;
	তার্কিক string_flag = মিছা;
	তার্কিক int_flag = মিছা, sint_flag = মিছা, uint_flag = মিছা;
	বাবে (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(পূর্ণাঙ্ক)
		FLAG(sint)
		FLAG(uint)
		ভঙ্গ;
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

	ধ্রুৱ RTLIL::Const &মান = cell->getParam(param_twine);

	যদি (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(মান.decode_string().c_str(), -1));
	} অন্যথা যদি (int_flag || uint_flag || sint_flag) {
		mp_int মান_mp;
		যদি (!const_to_mp_int(মান, &মান_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&মান_mp));
	} অন্যথা {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(মান.as_string().c_str(), -1));
	}
	ঘূৰাই TCL_OK;
}

স্থির পূর্ণাঙ্ক tcl_set_param(ClientData, Tcl_Interp *interp, পূর্ণাঙ্ক objc, Tcl_Obj *ধ্রুৱ objv[])
{
	পূর্ণাঙ্ক i;
	তার্কিক string_flag = মিছা, sint_flag = মিছা, uint_flag = মিছা;
	বাবে (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		ভঙ্গ;
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
			যদি (const_.size() < 32)
				const_.exts(32);
		} অন্যথা {
			যদি (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} অন্যথা {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ঘূৰাই TCL_OK;
}

পূর্ণাঙ্ক yosys_tcl_interp_init(Tcl_Interp *interp)
{
	যদি (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_বাৰ্তা(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, রিক্ত, রিক্ত);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, রিক্ত, রিক্ত);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, রিক্ত, রিক্ত);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, রিক্ত, রিক্ত);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, রিক্ত, রিক্ত);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, রিক্ত, রিক্ত);

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

	ঘূৰাই TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
