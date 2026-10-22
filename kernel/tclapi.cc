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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ "libs/json11/json11.hpp"

#ifdef YOSYS_ENABLE_TCL
#ଅନ୍ତର୍ଭାଵ <tcl.h>
#ଅନ୍ତର୍ଭାଵ <tclTomMath.h>
#ଅନ୍ତର୍ଭାଵ <tclTomMathDecls.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_TCL
ତାର୍କିକ yosys_tcl_repl_active = ମିଥ୍ୟା;

ଶୂନ୍ୟ yosys_tcl_activate_repl()
{
	yosys_tcl_repl_active = ସତ୍ୟ;
}

ସ୍ଥିର Tcl_Obj *json_to_tcl(Tcl_Interp *interp, ଅଚଳ json11::Json &json)
{
	ଯଦି (json.is_null())
		ଫେରନ୍ତୁ Tcl_NewStringObj("null", 4);
	ଅନ୍ୟଥା ଯଦି (json.is_string()) {
		auto string = json.string_ମୂଲ୍ୟ();
		ଫେରନ୍ତୁ Tcl_NewStringObj(string.data(), string.size());
	} ଅନ୍ୟଥା ଯଦି (json.is_ସଂଖ୍ୟା()) {
		ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ = json.ସଂଖ୍ୟା_ମୂଲ୍ୟ();
		ଦ୍ୱିଗୁଣ round_val = std::nearbyint(ମୂଲ୍ୟ);
		ଯଦି (std::isfinite(round_val) && ମୂଲ୍ୟ == round_val && ମୂଲ୍ୟ >= LONG_MIN && ମୂଲ୍ୟ < -ଦ୍ୱିଗୁଣ(LONG_MIN))
			ଫେରନ୍ତୁ Tcl_NewLongObj((long)round_val);
		ଅନ୍ୟଥା
			ଫେରନ୍ତୁ Tcl_NewDoubleObj(ମୂଲ୍ୟ);
	} ଅନ୍ୟଥା ଯଦି (json.is_bool()) {
		ଫେରନ୍ତୁ Tcl_NewBooleanObj(json.bool_ମୂଲ୍ୟ());
	} ଅନ୍ୟଥା ଯଦି (json.is_array()) {
		auto list = json.array_ବସ୍ତୁଗୁଡ଼ିକ();
		Tcl_Obj *result = Tcl_NewListObj(list.size(), nullptr);
		ପାଇଁ (auto &item : list)
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item));
		ଫେରନ୍ତୁ result;
	} ଅନ୍ୟଥା ଯଦି (json.is_object()) {
		auto map = json.object_ବସ୍ତୁଗୁଡ଼ିକ();
		Tcl_Obj *result = Tcl_NewListObj(map.size() * 2, nullptr);
		ପାଇଁ (auto &item : map) {
			Tcl_ListObjAppendElement(interp, result, Tcl_NewStringObj(item.first.data(), item.first.size()));
			Tcl_ListObjAppendElement(interp, result, json_to_tcl(interp, item.second));
		}
		ଫେରନ୍ତୁ result;
	} ଅନ୍ୟଥା {
		log_abort();
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_yosys_cmd(ClientData, Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅଚଳ ଅକ୍ଷର *argv[])
{
	std::vector<std::string> args;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < argc; i++)
		args.ଠେଲ_back(argv[i]);

	ଯଦି (args.size() >= 1 && args[0] == "-import") {
		ପାଇଁ (auto &it : pass_register) {
			std::string tcl_command_name = it.first;
			ଯଦି (tcl_command_name == "proc")
				tcl_command_name = "procs";
			ଅନ୍ୟଥା ଯଦି (tcl_command_name == "rename")
				tcl_command_name = "renames";
			Tcl_CmdInfo info;
			ଯଦି (Tcl_GetCommandInfo(interp, tcl_command_name.c_str(), &info) != 0) {
				log("[TCL: yosys -import] Command name collision: found pre-existing command `%s' -> skip.\n", it.first);
			} ଅନ୍ୟଥା {
				std::string tcl_script = stringf("proc %s args { yosys %s {*}$args }", tcl_command_name, it.first);
				Tcl_Eval(interp, tcl_script.c_str());
			}
		}
		ଫେରନ୍ତୁ TCL_OK;
	}

	yosys_get_design()->scratchpad_unset("result.json");
	yosys_get_design()->scratchpad_unset("result.string");

	ତାର୍କିକ in_repl = yosys_tcl_repl_active;
	auto guard = logger().error_throw_scope();
	ଚେଷ୍ଟା {
		ଯଦି (args.size() == 1) {
			Pass::call(yosys_get_design(), args[0]);
		} ଅନ୍ୟଥା {
			Pass::call(yosys_get_design(), args);
		}
	} ଧର (log_cmd_error_exception) {
		ଯଦି (in_repl) {
			auto design = yosys_get_design();
			ଯେତେବେଳେ (design->selection_ଗଦା.size() > 1)
				design->କାଢ଼_selection();
			log_reset_ଗଦା();
		}
		Tcl_SetResult(interp, (ଅକ୍ଷର *)"Yosys command produced an error", TCL_STATIC);

		yosys_tcl_repl_active = in_repl;
		ଫେରନ୍ତୁ TCL_ERROR;
	} ଧର (...) {
		log_error("uncaught exception during Yosys command invoked from TCL\n");
	}

	yosys_tcl_repl_active = in_repl;

	auto &scratchpad = yosys_get_design()->scratchpad;
	auto result = scratchpad.find("result.json");
	ଯଦି (result != scratchpad.end()) {
		std::string err;
		auto json = json11::Json::parse(result->second, err);
		ଯଦି (err.empty()) {
			Tcl_SetObjResult(interp, json_to_tcl(interp, json));
		} ଅନ୍ୟଥା
			log_warning("Ignoring result.json scratchpad value due to parse error: %s\n", err);
	} ଅନ୍ୟଥା ଯଦି ((result = scratchpad.find("result.string")) != scratchpad.end()) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(result->second.data(), result->second.size()));
	}

	ଫେରନ୍ତୁ TCL_OK;
}

#define FLAG(name) \
	ଯଦି (!strcmp(argv[i], "-" #name)) { \
		name##_flag = ସତ୍ୟ; \
		ଜାରି; \
	} \

#define FLAG2(name) \
	ଯଦି (!strcmp(Tcl_GetString(objv[i]), "-" #name)) { \
		name##_flag = ସତ୍ୟ; \
		ଜାରି; \
	} \

#define ERROR(str) \
	{ \
		Tcl_SetResult(interp, (ଅକ୍ଷର *)(str), TCL_STATIC); \
		ଫେରନ୍ତୁ TCL_ERROR; \
	}

ତାର୍କିକ const_to_mp_int(ଅଚଳ Const &a, mp_int *b, ତାର୍କିକ force_signed, ତାର୍କିକ force_unsigned)
{
	ଯଦି (!a.is_fully_def())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଯଦି (mp_init(b))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ତାର୍କିକ negative = ((a.flags & RTLIL::CONST_FLAG_SIGNED) || force_signed) &&
					!force_unsigned &&
					!a.empty() && (a.back() == RTLIL::S1);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = a.size() - 1; i >= 0; i--) {
		ଯଦି (mp_mul_2d(b, 1, b)) {
			mp_clear(b);
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		ଯଦି ((a[i] == RTLIL::S1) ^ negative) {
			ଯଦି (mp_add_d(b, 1, b)) {
				mp_clear(b);
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}
		}
	}

	ଯଦି (negative) {
		ଯଦି (mp_add_d(b, 1, b) || mp_neg(b, b)) {
			mp_clear(b);
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ mp_int_to_const(mp_int *a, Const &b, ତାର୍କିକ is_signed)
{
	ତାର୍କିକ negative = (mp_cmp_d(a, 0) == MP_LT);
	ଯଦି (negative && !is_signed)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଯଦି (negative) {
		ଯଦି (mp_neg(a, a) != MP_OKAY)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ଯଦି (mp_sub_d(a, 1, a) != MP_OKAY)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	std::vector<unsigned ଅକ୍ଷର> buf;
	buf.resize(mp_unsigned_bin_size(a));
	ଯଦି (mp_to_unsigned_bin(a, buf.data()) != MP_OKAY)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	Const::Builder b_bits(mp_ଗଣନା_bits(a) + is_signed);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mp_ଗଣନା_bits(a);) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < 8 && i < mp_ଗଣନା_bits(a); j++, i++) {
			ତାର୍କିକ bv = ((buf.back() & (1 << j)) != 0) ^ negative;
			b_bits.ଠେଲ_back(bv ? RTLIL::S1 : RTLIL::S0);
		}
		buf.କାଢ଼_back();
	}

	ଯଦି (is_signed) {
		b_bits.ଠେଲ_back(negative ? RTLIL::S1 : RTLIL::S0);
	}
	b = b_bits.build();

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_get_attr(ClientData, Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅଚଳ ଅକ୍ଷର *argv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ତାର୍କିକ mod_flag = ମିଥ୍ୟା, string_flag = ମିଥ୍ୟା, bool_flag = ମିଥ୍ୟା;
	ତାର୍କିକ int_flag = ମିଥ୍ୟା, sint_flag = ମିଥ୍ୟା, uint_flag = ମିଥ୍ୟା;
	ପାଇଁ (i = 1; i < argc; i++) {
		FLAG(mod)
		FLAG(string)
		FLAG(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
		FLAG(sint)
		FLAG(uint)
		FLAG(ତାର୍କିକ)
		ଭାଙ୍ଗ;
	}

	ଯଦି ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3) ||
			(string_flag + int_flag + sint_flag + uint_flag + bool_flag > 1))
		ERROR("bad usage: expected \"get_attr -mod [-string|-int|-sint|-uint|-bool] <module> <attrname>\""
			  " or \"get_attr [-string|-int|-sint|-uint|-bool] <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ଯଦି (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ଯଦି (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ଯଦି (mod_flag) {
		obj = mod;
	} ଅନ୍ୟଥା {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ଯଦି (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ଯଦି (!obj)
			obj = mod->cell(obj_twine);
		ଯଦି (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ଯଦି (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().find(attr_id);

	ଯଦି (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->get_string_attribute(attr_twine).c_str(), -1));
	} ଅନ୍ୟଥା ଯଦି (int_flag || uint_flag || sint_flag) {
		ଯଦି (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required for -int)");
		RTLIL::Const &ମୂଲ୍ୟ = obj->attributes.at(attr_twine);

		mp_int ମୂଲ୍ୟ_mp;
		ଯଦି (!const_to_mp_int(ମୂଲ୍ୟ, &ମୂଲ୍ୟ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&ମୂଲ୍ୟ_mp));
	} ଅନ୍ୟଥା ଯଦି (bool_flag) {
		Tcl_SetObjResult(interp, Tcl_NewBooleanObj(obj->get_bool_attribute(attr_twine)));
	} ଅନ୍ୟଥା {
		ଯଦି (!obj->has_attribute(attr_twine))
			ERROR("attribute missing (required unless -bool or -string)")

		Tcl_SetObjResult(interp, Tcl_NewStringObj(obj->attributes.at(attr_twine).as_string().c_str(), -1));
	}

	ଫେରନ୍ତୁ TCL_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_has_attr(ClientData, Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅଚଳ ଅକ୍ଷର *argv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ତାର୍କିକ mod_flag = ମିଥ୍ୟା;
	ପାଇଁ (i = 1; i < argc; i++) {
		FLAG(mod)
		ଭାଙ୍ଗ;
	}

	ଯଦି ((mod_flag && i != argc - 2) ||
			(!mod_flag && i != argc - 3))
		ERROR("bad usage: expected \"has_attr -mod <module> <attrname>\""
			  " or \"has_attr <module> <identifier> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	ଯଦି (!mod_flag)
		obj_id = RTLIL::escape_id(argv[i++]);
	attr_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ଯଦି (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ଯଦି (mod_flag) {
		obj = mod;
	} ଅନ୍ୟଥା {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ଯଦି (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ଯଦି (!obj)
			obj = mod->cell(obj_twine);
		ଯଦି (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ଯଦି (!obj)
		ERROR("object not found")

	Tcl_SetObjResult(interp, Tcl_NewStringObj(std::to_string(obj->has_attribute(yosys_design->twines().find(attr_id))).c_str(), -1));
	ଫେରନ୍ତୁ TCL_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_set_attr(ClientData, Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc, Tcl_Obj *ଅଚଳ objv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ତାର୍କିକ mod_flag = ମିଥ୍ୟା, string_flag = ମିଥ୍ୟା, bool_flag = ମିଥ୍ୟା;
	ତାର୍କିକ true_flag = ମିଥ୍ୟା, false_flag = ମିଥ୍ୟା, sint_flag = ମିଥ୍ୟା, uint_flag = ମିଥ୍ୟା;
	ପାଇଁ (i = 1; i < objc; i++) {
		FLAG2(mod)
		FLAG2(string)
		FLAG2(ସତ୍ୟ)
		FLAG2(ମିଥ୍ୟା)
		FLAG2(sint)
		FLAG2(uint)
		FLAG2(ତାର୍କିକ)
		ଭାଙ୍ଗ;
	}

	ଯଦି ((i != objc - (2 + !mod_flag + !(true_flag || false_flag))) ||
			(string_flag + sint_flag + uint_flag + bool_flag + true_flag + false_flag > 1))
		ERROR("bad usage: expected \"set_attr -mod [-string|-sint|-uint|-bool] <module> <attrname> <value>\""
			  " or \"set_attr [-string|-sint|-uint|-bool] <module> <identifier> <attrname> <value>\""
			  " or \"set_attr [-true|-false] <module> <identifier> <attrname>\""
			  " or \"set_attr -mod [-true|-false| <module> <attrname>\"")

	std::string mod_id, obj_id, attr_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	ଯଦି (!mod_flag)
		obj_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	attr_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ଯଦି (!mod)
		ERROR("module not found")

	RTLIL::AttrObject *obj = nullptr;
	ଯଦି (mod_flag) {
		obj = mod;
	} ଅନ୍ୟଥା {
		auto obj_twine = yosys_design->twines().find(obj_id);
		obj = mod->wire(obj_twine);
		ଯଦି (!obj)
			obj = mod->memories.at(obj_twine, nullptr);
		ଯଦି (!obj)
			obj = mod->cell(obj_twine);
		ଯଦି (!obj)
			obj = mod->processes.at(obj_twine, nullptr);
	}

	ଯଦି (!obj)
		ERROR("object not found")

	IdString attr_twine = yosys_design->twines().add(attr_id);

	ଯଦି (string_flag) {
		obj->set_string_attribute(attr_twine, Tcl_GetString(objv[i++]));
	} ଅନ୍ୟଥା ଯଦି (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ମୂଲ୍ୟ_mp;

		ଯଦି (Tcl_TakeBignumFromObj(interp, objv[i++], &ମୂଲ୍ୟ_mp))
			ERROR("non-integral value")

		ଯଦି (!mp_int_to_const(&ମୂଲ୍ୟ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ଯଦି (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ଯଦି (const_.size() < 32)
				const_.exts(32);
		} ଅନ୍ୟଥା {
			ଯଦି (const_.size() < 32)
				const_.extu(32);
		}

		obj->attributes[attr_twine] = const_;
	} ଅନ୍ୟଥା ଯଦି (bool_flag) {
		obj->set_bool_attribute(attr_twine, atoi(Tcl_GetString(objv[i++])) != 0);
	} ଅନ୍ୟଥା ଯଦି (true_flag) {
		obj->set_bool_attribute(attr_twine, ସତ୍ୟ);
	} ଅନ୍ୟଥା ଯଦି (false_flag) {
		obj->set_bool_attribute(attr_twine, ମିଥ୍ୟା);
	} ଅନ୍ୟଥା {
		obj->attributes[attr_twine] = Const::from_string(std::string(Tcl_GetString(objv[i++])));
	}

	ଫେରନ୍ତୁ TCL_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_get_param(ClientData, Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅଚଳ ଅକ୍ଷର *argv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ତାର୍କିକ string_flag = ମିଥ୍ୟା;
	ତାର୍କିକ int_flag = ମିଥ୍ୟା, sint_flag = ମିଥ୍ୟା, uint_flag = ମିଥ୍ୟା;
	ପାଇଁ (i = 1; i < argc; i++) {
		FLAG(string)
		FLAG(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
		FLAG(sint)
		FLAG(uint)
		ଭାଙ୍ଗ;
	}

	ଯଦି ((i != argc - 3) ||
			(string_flag + int_flag > 1))
		ERROR("bad usage: expected \"get_param [-string|-int|-sint|-uint] <module> <cellid> <paramname>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(argv[i++]);
	cell_id = RTLIL::escape_id(argv[i++]);
	param_id = RTLIL::escape_id(argv[i++]);

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ଯଦି (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	Cell* cell = mod->cell(cell_twine);
	ଯଦି (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().find(param_id);
	ଯଦି (!cell->hasParam(param_twine))
		ERROR("parameter missing")

	ଅଚଳ RTLIL::Const &ମୂଲ୍ୟ = cell->getParam(param_twine);

	ଯଦି (string_flag) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(ମୂଲ୍ୟ.decode_string().c_str(), -1));
	} ଅନ୍ୟଥା ଯଦି (int_flag || uint_flag || sint_flag) {
		mp_int ମୂଲ୍ୟ_mp;
		ଯଦି (!const_to_mp_int(ମୂଲ୍ୟ, &ମୂଲ୍ୟ_mp, sint_flag, uint_flag))
			ERROR("bignum manipulation failed");
		Tcl_SetObjResult(interp, Tcl_NewBignumObj(&ମୂଲ୍ୟ_mp));
	} ଅନ୍ୟଥା {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(ମୂଲ୍ୟ.as_string().c_str(), -1));
	}
	ଫେରନ୍ତୁ TCL_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_set_param(ClientData, Tcl_Interp *interp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objc, Tcl_Obj *ଅଚଳ objv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ତାର୍କିକ string_flag = ମିଥ୍ୟା, sint_flag = ମିଥ୍ୟା, uint_flag = ମିଥ୍ୟା;
	ପାଇଁ (i = 1; i < objc; i++) {
		FLAG2(string)
		FLAG2(sint)
		FLAG2(uint)
		ଭାଙ୍ଗ;
	}

	ଯଦି ((i != objc - 4) ||
			(string_flag + sint_flag + uint_flag > 1))
		ERROR("bad usage: expected \"set_param [-string|-sint|-uint] <module> <cellid> <paramname> <value>")

	std::string mod_id, cell_id, param_id;
	mod_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	cell_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));
	param_id = RTLIL::escape_id(Tcl_GetString(objv[i++]));

	auto mod_twine = yosys_design->twines().find(mod_id);
	RTLIL::Module *mod = yosys_design->module(mod_twine);
	ଯଦି (!mod)
		ERROR("module not found")

	auto cell_twine = yosys_design->twines().find(cell_id);
	RTLIL::Cell *cell = mod->cell(cell_twine);
	ଯଦି (!cell)
		ERROR("object not found")

	IdString param_twine = yosys_design->twines().add(param_id);

	ଯଦି (string_flag) {
		cell->setParam(param_twine, Const(std::string(Tcl_GetString(objv[i++]))));
	} ଅନ୍ୟଥା ଯଦି (sint_flag || uint_flag) {
		RTLIL::Const const_;
		mp_int ମୂଲ୍ୟ_mp;

		ଯଦି (Tcl_TakeBignumFromObj(interp, objv[i++], &ମୂଲ୍ୟ_mp))
			ERROR("non-integral value")

		ଯଦି (!mp_int_to_const(&ମୂଲ୍ୟ_mp, const_, sint_flag))
			ERROR("bignum manipulation failed");

		ଯଦି (sint_flag) {
			const_.flags |= RTLIL::CONST_FLAG_SIGNED;
			ଯଦି (const_.size() < 32)
				const_.exts(32);
		} ଅନ୍ୟଥା {
			ଯଦି (const_.size() < 32)
				const_.extu(32);
		}

		cell->setParam(param_twine, const_);
	} ଅନ୍ୟଥା {
		cell->setParam(param_twine, Const::from_string(std::string(Tcl_GetString(objv[i++]))));
	}
	ଫେରନ୍ତୁ TCL_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା yosys_tcl_interp_init(Tcl_Interp *interp)
{
	ଯଦି (Tcl_Init(interp)!=TCL_OK)
		log_warning("Tcl_Init() call failed - %s\n",Tcl_Errno_ସନ୍ଦେଶ(Tcl_GetErrno()));
	Tcl_CreateCommand(interp, "yosys", tcl_yosys_cmd, ଖାଲି, ଖାଲି);
	Tcl_CreateCommand(interp, "rtlil::get_attr", tcl_get_attr, ଖାଲି, ଖାଲି);
	Tcl_CreateCommand(interp, "rtlil::has_attr", tcl_has_attr, ଖାଲି, ଖାଲି);
	Tcl_CreateObjCommand(interp, "rtlil::set_attr", tcl_set_attr, ଖାଲି, ଖାଲି);
	Tcl_CreateCommand(interp, "rtlil::get_param", tcl_get_param, ଖାଲି, ଖାଲି);
	Tcl_CreateObjCommand(interp, "rtlil::set_param", tcl_set_param, ଖାଲି, ଖାଲି);

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

	ଫେରନ୍ତୁ TCL_OK ;
}
#endif

YOSYS_NAMESPACE_END
