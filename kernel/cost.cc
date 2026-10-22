#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned puur_naa_nka CellCosts::get(RTLIL::Module *mod)
{
	ᱡᱩᱫᱤ (mod_cost_cache_.count(mod->name))
		ᱨᱩᱣᱟᱹᱲ mod_cost_cache_.at(mod->name);

	unsigned puur_naa_nka module_cost = 1;
	for (auto c : mod->cells()) {
		unsigned puur_naa_nka new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ᱨᱩᱣᱟᱹᱲ module_cost;
}

static unsigned puur_naa_nka y_coef(RTLIL::IdString type)
{
	ᱡᱩᱫᱤ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ᱨᱩᱣᱟᱹᱲ 1;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type == ID($neg)) {
		ᱨᱩᱣᱟᱹᱲ 4;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type == ID($demux)) {
		ᱨᱩᱣᱟᱹᱲ 2;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type == ID($fa)) {
		ᱨᱩᱣᱟᱹᱲ 5;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ᱨᱩᱣᱟᱹᱲ 8;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type.in(ID($shl), ID($sshl))) {
		// left shift
		ᱨᱩᱣᱟᱹᱲ 10;
	}
	ᱨᱩᱣᱟᱹᱲ 0;
}

static unsigned puur_naa_nka max_inp_coef(RTLIL::IdString type)
{
	ᱡᱩᱫᱤ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ᱨᱩᱣᱟᱹᱲ 1;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ᱨᱩᱣᱟᱹᱲ 2;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type == ID($lcu)) {
		ᱨᱩᱣᱟᱹᱲ 5;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ᱨᱩᱣᱟᱹᱲ 7;
	}
	ᱨᱩᱣᱟᱹᱲ 0;
}

static unsigned puur_naa_nka sum_coef(RTLIL::IdString type)
{
	ᱡᱩᱫᱤ (type.in(ID($shr), ID($sshr))) {
		// right shift
		ᱨᱩᱣᱟᱹᱲ 4;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (type.in(ID($shift), ID($shiftx))) {
		// shift
		ᱨᱩᱣᱟᱹᱲ 8;
	}
	ᱨᱩᱣᱟᱹᱲ 0;
}

static unsigned puur_naa_nka is_div_mod(RTLIL::IdString type)
{
	ᱨᱩᱣᱟᱹᱲ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ᱨᱩᱣᱟᱹᱲ (
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

unsigned puur_naa_nka max_inp_width(RTLIL::Cell *cell)
{
	unsigned puur_naa_nka max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ᱡᱩᱫᱤ (cell->type == ID($bmux))
		ᱨᱩᱣᱟᱹᱲ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	for (RTLIL::IdString param : input_width_params)
		ᱡᱩᱫᱤ (cell->hasParam(param))
			max = std::max(max, (unsigned puur_naa_nka)cell->getParam(param).as_int());
	ᱨᱩᱣᱟᱹᱲ max;
}

unsigned puur_naa_nka port_width_sum(RTLIL::Cell *cell)
{
	unsigned puur_naa_nka sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	for (auto param : port_width_params)
		ᱡᱩᱫᱤ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ᱨᱩᱣᱟᱹᱲ sum;
}

unsigned puur_naa_nka CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ᱡᱩᱫᱤ (cmos_gate_cost().count(cell->type))
		ᱨᱩᱣᱟᱹᱲ 1;

	ᱡᱩᱫᱤ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ᱨᱩᱣᱟᱹᱲ get(design_->module(cell->type));
	} ᱵᱟᱝ ᱡᱩᱫᱤ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ᱨᱩᱣᱟᱹᱲ cell->getParam(ID::WIDTH).as_int();
	} ᱵᱟᱝ ᱡᱩᱫᱤ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ᱨᱩᱣᱟᱹᱲ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} ᱵᱟᱝ ᱡᱩᱫᱤ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		puur_naa_nka width = cell->getParam(param).as_int();
		ᱡᱩᱫᱤ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ᱨᱩᱣᱟᱹᱲ width * y_coef(cell->type);
	} ᱵᱟᱝ ᱡᱩᱫᱤ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned puur_naa_nka sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ᱨᱩᱣᱟᱹᱲ sum * sum_coef(cell->type);
	} ᱵᱟᱝ ᱡᱩᱫᱤ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned puur_naa_nka max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ᱨᱩᱣᱟᱹᱲ max * max_inp_coef(cell->type);
	} ᱵᱟᱝ ᱡᱩᱫᱤ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned puur_naa_nka sum = port_width_sum(cell);
		unsigned puur_naa_nka coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ᱨᱩᱣᱟᱹᱲ coef * sum * sum;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned puur_naa_nka cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned puur_naa_nka y_width = cell->getParam(ID::Y_WIDTH).as_int();
		for (auto &term: macc.terms) {
			ᱡᱩᱫᱤ (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned puur_naa_nka sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ᱨᱩᱣᱟᱹᱲ cost_sum;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (cell->type == ID($lut)) {
		puur_naa_nka width = cell->getParam(ID::WIDTH).as_int();
		unsigned puur_naa_nka cost = 1U << (unsigned puur_naa_nka)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ᱨᱩᱣᱟᱹᱲ cost;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (cell->type == ID($sop)) {
		puur_naa_nka width = cell->getParam(ID::WIDTH).as_int();
		puur_naa_nka depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ᱨᱩᱣᱟᱹᱲ (2 * width + 1) * depth;
	} ᱵᱟᱝ ᱡᱩᱫᱤ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ᱨᱩᱣᱟᱹᱲ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ᱨᱩᱣᱟᱹᱲ 1;
}
