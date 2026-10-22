#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned පූර්ණ_සංඛ්‍යා CellCosts::get(RTLIL::Module *mod)
{
	නම් (mod_cost_cache_.count(mod->name))
		ආපසු mod_cost_cache_.at(mod->name);

	unsigned පූර්ණ_සංඛ්‍යා module_cost = 1;
	සඳහා (auto c : mod->cells()) {
		unsigned පූර්ණ_සංඛ්‍යා new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ආපසු module_cost;
}

static unsigned පූර්ණ_සංඛ්‍යා y_coef(RTLIL::IdString type)
{
	නම් (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ආපසු 1;
	} නැතහොත් නම් (type == ID($neg)) {
		ආපසු 4;
	} නැතහොත් නම් (type == ID($demux)) {
		ආපසු 2;
	} නැතහොත් නම් (type == ID($fa)) {
		ආපසු 5;
	} නැතහොත් නම් (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ආපසු 8;
	} නැතහොත් නම් (type.in(ID($shl), ID($sshl))) {
		// left shift
		ආපසු 10;
	}
	ආපසු 0;
}

static unsigned පූර්ණ_සංඛ්‍යා max_inp_coef(RTLIL::IdString type)
{
	නම් (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ආපසු 1;
	} නැතහොත් නම් (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ආපසු 2;
	} නැතහොත් නම් (type == ID($lcu)) {
		ආපසු 5;
	} නැතහොත් නම් (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ආපසු 7;
	}
	ආපසු 0;
}

static unsigned පූර්ණ_සංඛ්‍යා sum_coef(RTLIL::IdString type)
{
	නම් (type.in(ID($shr), ID($sshr))) {
		// right shift
		ආපසු 4;
	} නැතහොත් නම් (type.in(ID($shift), ID($shiftx))) {
		// shift
		ආපසු 8;
	}
	ආපසු 0;
}

static unsigned පූර්ණ_සංඛ්‍යා is_div_mod(RTLIL::IdString type)
{
	ආපසු (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ආපසු (
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

unsigned පූර්ණ_සංඛ්‍යා max_inp_width(RTLIL::Cell *cell)
{
	unsigned පූර්ණ_සංඛ්‍යා max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	නම් (cell->type == ID($bmux))
		ආපසු cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	සඳහා (RTLIL::IdString param : input_width_params)
		නම් (cell->hasParam(param))
			max = std::max(max, (unsigned පූර්ණ_සංඛ්‍යා)cell->getParam(param).as_int());
	ආපසු max;
}

unsigned පූර්ණ_සංඛ්‍යා port_width_sum(RTLIL::Cell *cell)
{
	unsigned පූර්ණ_සංඛ්‍යා sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	සඳහා (auto param : port_width_params)
		නම් (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ආපසු sum;
}

unsigned පූර්ණ_සංඛ්‍යා CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	නම් (cmos_gate_cost().count(cell->type))
		ආපසු 1;

	නම් (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ආපසු get(design_->module(cell->type));
	} නැතහොත් නම් (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ආපසු cell->getParam(ID::WIDTH).as_int();
	} නැතහොත් නම් (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ආපසු cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} නැතහොත් නම් (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		පූර්ණ_සංඛ්‍යා width = cell->getParam(param).as_int();
		නම් (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ආපසු width * y_coef(cell->type);
	} නැතහොත් නම් (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned පූර්ණ_සංඛ්‍යා sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ආපසු sum * sum_coef(cell->type);
	} නැතහොත් නම් (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned පූර්ණ_සංඛ්‍යා max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ආපසු max * max_inp_coef(cell->type);
	} නැතහොත් නම් (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned පූර්ණ_සංඛ්‍යා sum = port_width_sum(cell);
		unsigned පූර්ණ_සංඛ්‍යා coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ආපසු coef * sum * sum;
	} නැතහොත් නම් (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned පූර්ණ_සංඛ්‍යා cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned පූර්ණ_සංඛ්‍යා y_width = cell->getParam(ID::Y_WIDTH).as_int();
		සඳහා (auto &term: macc.terms) {
			නම් (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned පූර්ණ_සංඛ්‍යා sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ආපසු cost_sum;
	} නැතහොත් නම් (cell->type == ID($lut)) {
		පූර්ණ_සංඛ්‍යා width = cell->getParam(ID::WIDTH).as_int();
		unsigned පූර්ණ_සංඛ්‍යා cost = 1U << (unsigned පූර්ණ_සංඛ්‍යා)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ආපසු cost;
	} නැතහොත් නම් (cell->type == ID($sop)) {
		පූර්ණ_සංඛ්‍යා width = cell->getParam(ID::WIDTH).as_int();
		පූර්ණ_සංඛ්‍යා depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ආපසු (2 * width + 1) * depth;
	} නැතහොත් නම් (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ආපසු 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ආපසු 1;
}
