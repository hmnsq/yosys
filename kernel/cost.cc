#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ကိန်းပြည့် CellCosts::get(RTLIL::Module *mod)
{
	အကယ်၍ (mod_cost_cache_.count(mod->name))
		ပြန်ပေး mod_cost_cache_.at(mod->name);

	unsigned ကိန်းပြည့် module_cost = 1;
	အတွက် (auto c : mod->cells()) {
		unsigned ကိန်းပြည့် new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ပြန်ပေး module_cost;
}

static unsigned ကိန်းပြည့် y_coef(RTLIL::IdString type)
{
	အကယ်၍ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ပြန်ပေး 1;
	} မဟုတ်ပါက အကယ်၍ (type == ID($neg)) {
		ပြန်ပေး 4;
	} မဟုတ်ပါက အကယ်၍ (type == ID($demux)) {
		ပြန်ပေး 2;
	} မဟုတ်ပါက အကယ်၍ (type == ID($fa)) {
		ပြန်ပေး 5;
	} မဟုတ်ပါက အကယ်၍ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ပြန်ပေး 8;
	} မဟုတ်ပါက အကယ်၍ (type.in(ID($shl), ID($sshl))) {
		// left shift
		ပြန်ပေး 10;
	}
	ပြန်ပေး 0;
}

static unsigned ကိန်းပြည့် max_inp_coef(RTLIL::IdString type)
{
	အကယ်၍ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ပြန်ပေး 1;
	} မဟုတ်ပါက အကယ်၍ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ပြန်ပေး 2;
	} မဟုတ်ပါက အကယ်၍ (type == ID($lcu)) {
		ပြန်ပေး 5;
	} မဟုတ်ပါက အကယ်၍ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ပြန်ပေး 7;
	}
	ပြန်ပေး 0;
}

static unsigned ကိန်းပြည့် sum_coef(RTLIL::IdString type)
{
	အကယ်၍ (type.in(ID($shr), ID($sshr))) {
		// right shift
		ပြန်ပေး 4;
	} မဟုတ်ပါက အကယ်၍ (type.in(ID($shift), ID($shiftx))) {
		// shift
		ပြန်ပေး 8;
	}
	ပြန်ပေး 0;
}

static unsigned ကိန်းပြည့် is_div_mod(RTLIL::IdString type)
{
	ပြန်ပေး (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ပြန်ပေး (
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

unsigned ကိန်းပြည့် max_inp_width(RTLIL::Cell *cell)
{
	unsigned ကိန်းပြည့် max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	အကယ်၍ (cell->type == ID($bmux))
		ပြန်ပေး cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	အတွက် (RTLIL::IdString param : input_width_params)
		အကယ်၍ (cell->hasParam(param))
			max = std::max(max, (unsigned ကိန်းပြည့်)cell->getParam(param).as_int());
	ပြန်ပေး max;
}

unsigned ကိန်းပြည့် port_width_sum(RTLIL::Cell *cell)
{
	unsigned ကိန်းပြည့် sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	အတွက် (auto param : port_width_params)
		အကယ်၍ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ပြန်ပေး sum;
}

unsigned ကိန်းပြည့် CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	အကယ်၍ (cmos_gate_cost().count(cell->type))
		ပြန်ပေး 1;

	အကယ်၍ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ပြန်ပေး get(design_->module(cell->type));
	} မဟုတ်ပါက အကယ်၍ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ပြန်ပေး cell->getParam(ID::WIDTH).as_int();
	} မဟုတ်ပါက အကယ်၍ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ပြန်ပေး cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} မဟုတ်ပါက အကယ်၍ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ကိန်းပြည့် width = cell->getParam(param).as_int();
		အကယ်၍ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ပြန်ပေး width * y_coef(cell->type);
	} မဟုတ်ပါက အကယ်၍ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ကိန်းပြည့် sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ပြန်ပေး sum * sum_coef(cell->type);
	} မဟုတ်ပါက အကယ်၍ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ကိန်းပြည့် max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ပြန်ပေး max * max_inp_coef(cell->type);
	} မဟုတ်ပါက အကယ်၍ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ကိန်းပြည့် sum = port_width_sum(cell);
		unsigned ကိန်းပြည့် coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ပြန်ပေး coef * sum * sum;
	} မဟုတ်ပါက အကယ်၍ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ကိန်းပြည့် cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ကိန်းပြည့် y_width = cell->getParam(ID::Y_WIDTH).as_int();
		အတွက် (auto &term: macc.terms) {
			အကယ်၍ (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ကိန်းပြည့် sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ပြန်ပေး cost_sum;
	} မဟုတ်ပါက အကယ်၍ (cell->type == ID($lut)) {
		ကိန်းပြည့် width = cell->getParam(ID::WIDTH).as_int();
		unsigned ကိန်းပြည့် cost = 1U << (unsigned ကိန်းပြည့်)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ပြန်ပေး cost;
	} မဟုတ်ပါက အကယ်၍ (cell->type == ID($sop)) {
		ကိန်းပြည့် width = cell->getParam(ID::WIDTH).as_int();
		ကိန်းပြည့် depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ပြန်ပေး (2 * width + 1) * depth;
	} မဟုတ်ပါက အကယ်၍ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ပြန်ပေး 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ပြန်ပေး 1;
}
