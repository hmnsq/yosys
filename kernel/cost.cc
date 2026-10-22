#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ចំនួនគត់ CellCosts::get(RTLIL::Module *mod)
{
	ប្រសិនបើ (mod_cost_cache_.count(mod->name))
		ត្រឡប់ mod_cost_cache_.at(mod->name);

	unsigned ចំនួនគត់ module_cost = 1;
	សម្រាប់ (auto c : mod->cells()) {
		unsigned ចំនួនគត់ new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ត្រឡប់ module_cost;
}

static unsigned ចំនួនគត់ y_coef(RTLIL::IdString type)
{
	ប្រសិនបើ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ត្រឡប់ 1;
	} បើមិនដូច្នេះ ប្រសិនបើ (type == ID($neg)) {
		ត្រឡប់ 4;
	} បើមិនដូច្នេះ ប្រសិនបើ (type == ID($demux)) {
		ត្រឡប់ 2;
	} បើមិនដូច្នេះ ប្រសិនបើ (type == ID($fa)) {
		ត្រឡប់ 5;
	} បើមិនដូច្នេះ ប្រសិនបើ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ត្រឡប់ 8;
	} បើមិនដូច្នេះ ប្រសិនបើ (type.in(ID($shl), ID($sshl))) {
		// left shift
		ត្រឡប់ 10;
	}
	ត្រឡប់ 0;
}

static unsigned ចំនួនគត់ max_inp_coef(RTLIL::IdString type)
{
	ប្រសិនបើ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ត្រឡប់ 1;
	} បើមិនដូច្នេះ ប្រសិនបើ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ត្រឡប់ 2;
	} បើមិនដូច្នេះ ប្រសិនបើ (type == ID($lcu)) {
		ត្រឡប់ 5;
	} បើមិនដូច្នេះ ប្រសិនបើ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ត្រឡប់ 7;
	}
	ត្រឡប់ 0;
}

static unsigned ចំនួនគត់ sum_coef(RTLIL::IdString type)
{
	ប្រសិនបើ (type.in(ID($shr), ID($sshr))) {
		// right shift
		ត្រឡប់ 4;
	} បើមិនដូច្នេះ ប្រសិនបើ (type.in(ID($shift), ID($shiftx))) {
		// shift
		ត្រឡប់ 8;
	}
	ត្រឡប់ 0;
}

static unsigned ចំនួនគត់ is_div_mod(RTLIL::IdString type)
{
	ត្រឡប់ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ត្រឡប់ (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($assert), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned ចំនួនគត់ max_inp_width(RTLIL::Cell *cell)
{
	unsigned ចំនួនគត់ max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ប្រសិនបើ (cell->type == ID($bmux))
		ត្រឡប់ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	សម្រាប់ (RTLIL::IdString param : input_width_params)
		ប្រសិនបើ (cell->hasParam(param))
			max = std::max(max, (unsigned ចំនួនគត់)cell->getParam(param).as_int());
	ត្រឡប់ max;
}

unsigned ចំនួនគត់ port_width_sum(RTLIL::Cell *cell)
{
	unsigned ចំនួនគត់ sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	សម្រាប់ (auto param : port_width_params)
		ប្រសិនបើ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ត្រឡប់ sum;
}

unsigned ចំនួនគត់ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ប្រសិនបើ (cmos_gate_cost().count(cell->type))
		ត្រឡប់ 1;

	ប្រសិនបើ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ត្រឡប់ get(design_->module(cell->type));
	} បើមិនដូច្នេះ ប្រសិនបើ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ត្រឡប់ cell->getParam(ID::WIDTH).as_int();
	} បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ត្រឡប់ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} បើមិនដូច្នេះ ប្រសិនបើ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ចំនួនគត់ width = cell->getParam(param).as_int();
		ប្រសិនបើ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ត្រឡប់ width * y_coef(cell->type);
	} បើមិនដូច្នេះ ប្រសិនបើ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ចំនួនគត់ sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ត្រឡប់ sum * sum_coef(cell->type);
	} បើមិនដូច្នេះ ប្រសិនបើ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ចំនួនគត់ max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ត្រឡប់ max * max_inp_coef(cell->type);
	} បើមិនដូច្នេះ ប្រសិនបើ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ចំនួនគត់ sum = port_width_sum(cell);
		unsigned ចំនួនគត់ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ត្រឡប់ coef * sum * sum;
	} បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ចំនួនគត់ cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ចំនួនគត់ y_width = cell->getParam(ID::Y_WIDTH).as_int();
		សម្រាប់ (auto &term: macc.terms) {
			ប្រសិនបើ (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ចំនួនគត់ sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ត្រឡប់ cost_sum;
	} បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($lut)) {
		ចំនួនគត់ width = cell->getParam(ID::WIDTH).as_int();
		unsigned ចំនួនគត់ cost = 1U << (unsigned ចំនួនគត់)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ត្រឡប់ cost;
	} បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($sop)) {
		ចំនួនគត់ width = cell->getParam(ID::WIDTH).as_int();
		ចំនួនគត់ depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ត្រឡប់ (2 * width + 1) * depth;
	} បើមិនដូច្នេះ ប្រសិនបើ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ត្រឡប់ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ត្រឡប់ 1;
}
