#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned số_nguyên CellCosts::get(RTLIL::Module *mod)
{
	nếu (mod_cost_cache_.count(mod->name))
		trả_về mod_cost_cache_.at(mod->name);

	unsigned số_nguyên module_cost = 1;
	cho (auto c : mod->cells()) {
		unsigned số_nguyên new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	trả_về module_cost;
}

static unsigned số_nguyên y_coef(RTLIL::IdString type)
{
	nếu (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		trả_về 1;
	} không_thì nếu (type == ID($neg)) {
		trả_về 4;
	} không_thì nếu (type == ID($demux)) {
		trả_về 2;
	} không_thì nếu (type == ID($fa)) {
		trả_về 5;
	} không_thì nếu (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		trả_về 8;
	} không_thì nếu (type.in(ID($shl), ID($sshl))) {
		// left shift
		trả_về 10;
	}
	trả_về 0;
}

static unsigned số_nguyên max_inp_coef(RTLIL::IdString type)
{
	nếu (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		trả_về 1;
	} không_thì nếu (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		trả_về 2;
	} không_thì nếu (type == ID($lcu)) {
		trả_về 5;
	} không_thì nếu (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		trả_về 7;
	}
	trả_về 0;
}

static unsigned số_nguyên sum_coef(RTLIL::IdString type)
{
	nếu (type.in(ID($shr), ID($sshr))) {
		// right shift
		trả_về 4;
	} không_thì nếu (type.in(ID($shift), ID($shiftx))) {
		// shift
		trả_về 8;
	}
	trả_về 0;
}

static unsigned số_nguyên is_div_mod(RTLIL::IdString type)
{
	trả_về (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	trả_về (
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

unsigned số_nguyên max_inp_width(RTLIL::Cell *cell)
{
	unsigned số_nguyên max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	nếu (cell->type == ID($bmux))
		trả_về cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	cho (RTLIL::IdString param : input_width_params)
		nếu (cell->hasParam(param))
			max = std::max(max, (unsigned số_nguyên)cell->getParam(param).as_int());
	trả_về max;
}

unsigned số_nguyên port_width_sum(RTLIL::Cell *cell)
{
	unsigned số_nguyên sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	cho (auto param : port_width_params)
		nếu (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	trả_về sum;
}

unsigned số_nguyên CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	nếu (cmos_gate_cost().count(cell->type))
		trả_về 1;

	nếu (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		trả_về get(design_->module(cell->type));
	} không_thì nếu (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		trả_về cell->getParam(ID::WIDTH).as_int();
	} không_thì nếu (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		trả_về cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} không_thì nếu (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		số_nguyên width = cell->getParam(param).as_int();
		nếu (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		trả_về width * y_coef(cell->type);
	} không_thì nếu (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned số_nguyên sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		trả_về sum * sum_coef(cell->type);
	} không_thì nếu (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned số_nguyên max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		trả_về max * max_inp_coef(cell->type);
	} không_thì nếu (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned số_nguyên sum = port_width_sum(cell);
		unsigned số_nguyên coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		trả_về coef * sum * sum;
	} không_thì nếu (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned số_nguyên cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned số_nguyên y_width = cell->getParam(ID::Y_WIDTH).as_int();
		cho (auto &term: macc.terms) {
			nếu (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned số_nguyên sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		trả_về cost_sum;
	} không_thì nếu (cell->type == ID($lut)) {
		số_nguyên width = cell->getParam(ID::WIDTH).as_int();
		unsigned số_nguyên cost = 1U << (unsigned số_nguyên)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		trả_về cost;
	} không_thì nếu (cell->type == ID($sop)) {
		số_nguyên width = cell->getParam(ID::WIDTH).as_int();
		số_nguyên depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		trả_về (2 * width + 1) * depth;
	} không_thì nếu (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		trả_về 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	trả_về 1;
}
