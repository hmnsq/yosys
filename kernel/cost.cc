#ضمن "kernel/cost.h"
#ضمن "kernel/macc.h"

USING_YOSYS_NAMESPACE

بینښه صحيح CellCosts::get(RTLIL::Module *mod)
{
	إذا (mod_لګښت_cache.شمېر(mod->name))
		أرجع mod_لګښت_cache.at(mod->name);

	بینښه صحيح module_لګښت = 1;
	لكل (خپلکاری c : mod->cells()) {
		بینښه صحيح new_لګښت = module_لګښت + get(c);
		module_لګښت = new_لګښت >= module_لګښت ? new_لګښت : لوی_عدد;
	}

	mod_لګښت_cache[mod->name] = module_لګښت;
	أرجع module_لګښت;
}

ساكن بینښه صحيح y_coef(RTLIL::IdString type)
{
	إذا (
	  // equality
	  type.دننه(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.دننه(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.دننه(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		أرجع 1;
	} وإلا إذا (type == ID($neg)) {
		أرجع 4;
	} وإلا إذا (type == ID($demux)) {
		أرجع 2;
	} وإلا إذا (type == ID($fa)) {
		أرجع 5;
	} وإلا إذا (type.دننه(ID($جمع_کړه), ID($sub), ID($alu))) {
		// multi-bit adders
		أرجع 8;
	} وإلا إذا (type.دننه(ID($shl), ID($sshl))) {
		// left shift
		أرجع 10;
	}
	أرجع 0;
}

ساكن بینښه صحيح max_inp_coef(RTLIL::IdString type)
{
	إذا (
	  // binop reduce
	  type.دننه(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.دننه(ID($logic_not), ID($pmux), ID($bmux))) {
		أرجع 1;
	} وإلا إذا (
	  // equality
	  type.دننه(ID($eq), ID($ne)) ||
	  // logic
	  type.دننه(ID($logic_and), ID($logic_or))) {
		أرجع 2;
	} وإلا إذا (type == ID($lcu)) {
		أرجع 5;
	} وإلا إذا (type.دننه(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		أرجع 7;
	}
	أرجع 0;
}

ساكن بینښه صحيح مجموعه_coef(RTLIL::IdString type)
{
	إذا (type.دننه(ID($shr), ID($sshr))) {
		// right shift
		أرجع 4;
	} وإلا إذا (type.دننه(ID($ښويدنه), ID($shiftx))) {
		//ښويدنه
		أرجع 8;
	}
	أرجع 0;
}

ساكن بینښه صحيح is_div_mod(RTLIL::IdString type)
{
	أرجع (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ساكن منطقي is_free(RTLIL::IdString type)
{
	أرجع (
	  //نښانونه
	  type.دننه(ID($overwrite_نښان), ID($سېټ_نښان), ID($original_نښان), ID($get_نښان)) ||
	  // formal
	  type.دننه(ID($check), ID($equiv), ID($initstate), ID($أكد), ID($assume), ID($ژوندی), ID($cover), ID($fair)) ||
	  type.دننه(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.دننه(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.دننه(ID($concat), ID($ټوټه), ID($pos)) ||
	  // specify
	  type.دننه(ID($specrule), ID($specify2), ID($specify3)));
}

بینښه صحيح max_inp_پلنوالی(RTLIL::Cell *cell)
{
	بینښه صحيح max = 0;
	RTLIL::IdString ننوت_پلنوالی_params[] = {
	  ID::پلنوالی_2,
	  ID::A_پلنوالی,
	  ID::B_پلنوالی,
	  ID::S_پلنوالی,
	};

	إذا (cell->type == ID($bmux))
		أرجع cell->getParam(ID::پلنوالی_2).as_int() << cell->getParam(ID::S_پلنوالی).as_int();

	لكل (RTLIL::IdString param : ننوت_پلنوالی_params)
		إذا (cell->hasParam(param))
			max = std::max(max, (بینښه صحيح)cell->getParam(param).as_int());
	أرجع max;
}

بینښه صحيح پورټ_پلنوالی_مجموعه(RTLIL::Cell *cell)
{
	بینښه صحيح مجموعه = 0;
	RTLIL::IdString پورټ_پلنوالی_params[] = {
	  ID::پلنوالی_2, ID::A_پلنوالی, ID::B_پلنوالی, ID::S_پلنوالی, ID::Y_پلنوالی,
	};

	لكل (خپلکاری param : پورټ_پلنوالی_params)
		إذا (cell->hasParam(param))
			مجموعه += cell->getParam(param).as_int();

	أرجع مجموعه;
}

بینښه صحيح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	إذا (cmos_gate_لګښت().شمېر(cell->type))
		أرجع 1;

	إذا (design_ && design_->module(cell->type) && cell->parameters.تشه()) {
		log_debug("%s is a module, recurse\n", cell->name);
		أرجع get(design_->module(cell->type));
	} وإلا إذا (cell->is_builtin_ff()) {
		log_assert(cell->has_پورټ(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		أرجع cell->getParam(ID::پلنوالی_2).as_int();
	} وإلا إذا (cell->type.دننه(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		أرجع cell->getParam(ID::پلنوالی_2).as_int() * cell->getParam(ID::اندازه_2).as_int();
	} وإلا إذا (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_پلنوالی) || cell->hasParam(ID::پلنوالی_2)) && "Unknown width");
		خپلکاری param = cell->hasParam(ID::Y_پلنوالی) ? ID::Y_پلنوالی : ID::پلنوالی_2;
		صحيح پلنوالی = cell->getParam(param).as_int();
		إذا (cell->type == ID($demux))
			پلنوالی <<= cell->getParam(ID::S_پلنوالی).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, پلنوالی, y_coef(cell->type));
		أرجع پلنوالی * y_coef(cell->type);
	} وإلا إذا (مجموعه_coef(cell->type)) {
		// linear with sum of port widths
		بینښه صحيح مجموعه = پورټ_پلنوالی_مجموعه(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, مجموعه, مجموعه_coef(cell->type));
		أرجع مجموعه * مجموعه_coef(cell->type);
	} وإلا إذا (max_inp_coef(cell->type)) {
		// linear with largest input width
		بینښه صحيح max = max_inp_پلنوالی(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		أرجع max * max_inp_coef(cell->type);
	} وإلا إذا (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		بینښه صحيح مجموعه = پورټ_پلنوالی_مجموعه(cell);
		بینښه صحيح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, مجموعه * مجموعه);
		أرجع coef * مجموعه * مجموعه;
	} وإلا إذا (cell->type.دننه(ID($macc), ID($macc_v2))) {
		// quadratic per term
		بینښه صحيح لګښت_مجموعه = 0;
		Macc macc;
		macc.from_cell(cell);
		بینښه صحيح y_پلنوالی = cell->getParam(ID::Y_پلنوالی).as_int();
		لكل (خپلکاری &غړی: macc.terms) {
			إذا (غړی.دننه_b.تشه()) {
				// neglect addends
				استمر;
			}
			بینښه a_پلنوالی = غړی.دننه_a.اندازه(), b_پلنوالی = غړی.دننه_b.اندازه();
			بینښه صحيح مجموعه = a_پلنوالی + b_پلنوالی + std::min(y_پلنوالی, a_پلنوالی + b_پلنوالی);
			لګښت_مجموعه += 3 * مجموعه * مجموعه;
		}
		أرجع لګښت_مجموعه;
	} وإلا إذا (cell->type == ID($lut)) {
		صحيح پلنوالی = cell->getParam(ID::پلنوالی_2).as_int();
		بینښه صحيح لګښت = 1U << (بینښه صحيح)پلنوالی;
		log_debug("%s is 2**%d\n", cell->name, پلنوالی);
		أرجع لګښت;
	} وإلا إذا (cell->type == ID($sop)) {
		صحيح پلنوالی = cell->getParam(ID::پلنوالی_2).as_int();
		صحيح ژوروالی = cell->getParam(ID::ژوروالی_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, پلنوالی, ژوروالی);
		أرجع (2 * پلنوالی + 1) * ژوروالی;
	} وإلا إذا (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		أرجع 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_اندازه(cell->parameters));
	أرجع 1;
}
