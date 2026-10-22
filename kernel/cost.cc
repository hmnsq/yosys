#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned целое CellCosts::get(RTLIL::Module *mod)
{
	если (mod_cost_cache_.count(mod->name))
		вернуть mod_cost_cache_.at(mod->name);

	unsigned целое module_cost = 1;
	для (auto c : mod->cells()) {
		unsigned целое new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	вернуть module_cost;
}

static unsigned целое y_coef(RTLIL::IdString type)
{
	если (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		вернуть 1;
	} иначе если (type == ID($neg)) {
		вернуть 4;
	} иначе если (type == ID($demux)) {
		вернуть 2;
	} иначе если (type == ID($fa)) {
		вернуть 5;
	} иначе если (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		вернуть 8;
	} иначе если (type.in(ID($shl), ID($sshl))) {
		// left shift
		вернуть 10;
	}
	вернуть 0;
}

static unsigned целое max_inp_coef(RTLIL::IdString type)
{
	если (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		вернуть 1;
	} иначе если (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		вернуть 2;
	} иначе если (type == ID($lcu)) {
		вернуть 5;
	} иначе если (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		вернуть 7;
	}
	вернуть 0;
}

static unsigned целое sum_coef(RTLIL::IdString type)
{
	если (type.in(ID($shr), ID($sshr))) {
		// right shift
		вернуть 4;
	} иначе если (type.in(ID($shift), ID($shiftx))) {
		// shift
		вернуть 8;
	}
	вернуть 0;
}

static unsigned целое is_div_mod(RTLIL::IdString type)
{
	вернуть (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	вернуть (
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

unsigned целое max_inp_width(RTLIL::Cell *cell)
{
	unsigned целое max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	если (cell->type == ID($bmux))
		вернуть cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	для (RTLIL::IdString param : input_width_params)
		если (cell->hasParam(param))
			max = std::max(max, (unsigned целое)cell->getParam(param).as_int());
	вернуть max;
}

unsigned целое port_width_sum(RTLIL::Cell *cell)
{
	unsigned целое sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	для (auto param : port_width_params)
		если (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	вернуть sum;
}

unsigned целое CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	если (cmos_gate_cost().count(cell->type))
		вернуть 1;

	если (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		вернуть get(design_->module(cell->type));
	} иначе если (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		вернуть cell->getParam(ID::WIDTH).as_int();
	} иначе если (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		вернуть cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} иначе если (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		целое width = cell->getParam(param).as_int();
		если (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		вернуть width * y_coef(cell->type);
	} иначе если (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned целое sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		вернуть sum * sum_coef(cell->type);
	} иначе если (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned целое max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		вернуть max * max_inp_coef(cell->type);
	} иначе если (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned целое sum = port_width_sum(cell);
		unsigned целое coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		вернуть coef * sum * sum;
	} иначе если (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned целое cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned целое y_width = cell->getParam(ID::Y_WIDTH).as_int();
		для (auto &term: macc.terms) {
			если (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned целое sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		вернуть cost_sum;
	} иначе если (cell->type == ID($lut)) {
		целое width = cell->getParam(ID::WIDTH).as_int();
		unsigned целое cost = 1U << (unsigned целое)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		вернуть cost;
	} иначе если (cell->type == ID($sop)) {
		целое width = cell->getParam(ID::WIDTH).as_int();
		целое depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		вернуть (2 * width + 1) * depth;
	} иначе если (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		вернуть 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	вернуть 1;
}
