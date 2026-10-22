#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ຈຳນວນເຕັມ CellCosts::get(RTLIL::Module *mod)
{
	ຖ້າ (mod_cost_cache_.count(mod->name))
		ສົ່ງຄືນ mod_cost_cache_.at(mod->name);

	unsigned ຈຳນວນເຕັມ module_cost = 1;
	ສຳລັບ (auto c : mod->cells()) {
		unsigned ຈຳນວນເຕັມ new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ສົ່ງຄືນ module_cost;
}

static unsigned ຈຳນວນເຕັມ y_coef(RTLIL::IdString type)
{
	ຖ້າ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ສົ່ງຄືນ 1;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == ID($neg)) {
		ສົ່ງຄືນ 4;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == ID($demux)) {
		ສົ່ງຄືນ 2;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == ID($fa)) {
		ສົ່ງຄືນ 5;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ສົ່ງຄືນ 8;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type.in(ID($shl), ID($sshl))) {
		// left shift
		ສົ່ງຄືນ 10;
	}
	ສົ່ງຄືນ 0;
}

static unsigned ຈຳນວນເຕັມ max_inp_coef(RTLIL::IdString type)
{
	ຖ້າ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ສົ່ງຄືນ 1;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ສົ່ງຄືນ 2;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == ID($lcu)) {
		ສົ່ງຄືນ 5;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ສົ່ງຄືນ 7;
	}
	ສົ່ງຄືນ 0;
}

static unsigned ຈຳນວນເຕັມ sum_coef(RTLIL::IdString type)
{
	ຖ້າ (type.in(ID($shr), ID($sshr))) {
		// right shift
		ສົ່ງຄືນ 4;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (type.in(ID($shift), ID($shiftx))) {
		// shift
		ສົ່ງຄືນ 8;
	}
	ສົ່ງຄືນ 0;
}

static unsigned ຈຳນວນເຕັມ is_div_mod(RTLIL::IdString type)
{
	ສົ່ງຄືນ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ສົ່ງຄືນ (
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

unsigned ຈຳນວນເຕັມ max_inp_width(RTLIL::Cell *cell)
{
	unsigned ຈຳນວນເຕັມ max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ຖ້າ (cell->type == ID($bmux))
		ສົ່ງຄືນ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	ສຳລັບ (RTLIL::IdString param : input_width_params)
		ຖ້າ (cell->hasParam(param))
			max = std::max(max, (unsigned ຈຳນວນເຕັມ)cell->getParam(param).as_int());
	ສົ່ງຄືນ max;
}

unsigned ຈຳນວນເຕັມ port_width_sum(RTLIL::Cell *cell)
{
	unsigned ຈຳນວນເຕັມ sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	ສຳລັບ (auto param : port_width_params)
		ຖ້າ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ສົ່ງຄືນ sum;
}

unsigned ຈຳນວນເຕັມ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ຖ້າ (cmos_gate_cost().count(cell->type))
		ສົ່ງຄືນ 1;

	ຖ້າ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ສົ່ງຄືນ get(design_->module(cell->type));
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ສົ່ງຄືນ cell->getParam(ID::WIDTH).as_int();
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ສົ່ງຄືນ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ຈຳນວນເຕັມ width = cell->getParam(param).as_int();
		ຖ້າ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ສົ່ງຄືນ width * y_coef(cell->type);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ຈຳນວນເຕັມ sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ສົ່ງຄືນ sum * sum_coef(cell->type);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ຈຳນວນເຕັມ max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ສົ່ງຄືນ max * max_inp_coef(cell->type);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ຈຳນວນເຕັມ sum = port_width_sum(cell);
		unsigned ຈຳນວນເຕັມ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ສົ່ງຄືນ coef * sum * sum;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ຈຳນວນເຕັມ cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ຈຳນວນເຕັມ y_width = cell->getParam(ID::Y_WIDTH).as_int();
		ສຳລັບ (auto &term: macc.terms) {
			ຖ້າ (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ຈຳນວນເຕັມ sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ສົ່ງຄືນ cost_sum;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cell->type == ID($lut)) {
		ຈຳນວນເຕັມ width = cell->getParam(ID::WIDTH).as_int();
		unsigned ຈຳນວນເຕັມ cost = 1U << (unsigned ຈຳນວນເຕັມ)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ສົ່ງຄືນ cost;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cell->type == ID($sop)) {
		ຈຳນວນເຕັມ width = cell->getParam(ID::WIDTH).as_int();
		ຈຳນວນເຕັມ depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ສົ່ງຄືນ (2 * width + 1) * depth;
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ສົ່ງຄືນ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ສົ່ງຄືນ 1;
}
