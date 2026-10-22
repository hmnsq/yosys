#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned จำนวนเต็ม CellCosts::get(RTLIL::Module *mod)
{
	ถ้า (mod_cost_cache_.count(mod->name))
		คืน mod_cost_cache_.at(mod->name);

	unsigned จำนวนเต็ม module_cost = 1;
	สำหรับ (auto c : mod->cells()) {
		unsigned จำนวนเต็ม new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	คืน module_cost;
}

static unsigned จำนวนเต็ม y_coef(RTLIL::IdString type)
{
	ถ้า (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		คืน 1;
	} มิฉะนั้น ถ้า (type == ID($neg)) {
		คืน 4;
	} มิฉะนั้น ถ้า (type == ID($demux)) {
		คืน 2;
	} มิฉะนั้น ถ้า (type == ID($fa)) {
		คืน 5;
	} มิฉะนั้น ถ้า (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		คืน 8;
	} มิฉะนั้น ถ้า (type.in(ID($shl), ID($sshl))) {
		// left shift
		คืน 10;
	}
	คืน 0;
}

static unsigned จำนวนเต็ม max_inp_coef(RTLIL::IdString type)
{
	ถ้า (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		คืน 1;
	} มิฉะนั้น ถ้า (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		คืน 2;
	} มิฉะนั้น ถ้า (type == ID($lcu)) {
		คืน 5;
	} มิฉะนั้น ถ้า (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		คืน 7;
	}
	คืน 0;
}

static unsigned จำนวนเต็ม sum_coef(RTLIL::IdString type)
{
	ถ้า (type.in(ID($shr), ID($sshr))) {
		// right shift
		คืน 4;
	} มิฉะนั้น ถ้า (type.in(ID($shift), ID($shiftx))) {
		// shift
		คืน 8;
	}
	คืน 0;
}

static unsigned จำนวนเต็ม is_div_mod(RTLIL::IdString type)
{
	คืน (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	คืน (
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

unsigned จำนวนเต็ม max_inp_width(RTLIL::Cell *cell)
{
	unsigned จำนวนเต็ม max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ถ้า (cell->type == ID($bmux))
		คืน cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	สำหรับ (RTLIL::IdString param : input_width_params)
		ถ้า (cell->hasParam(param))
			max = std::max(max, (unsigned จำนวนเต็ม)cell->getParam(param).as_int());
	คืน max;
}

unsigned จำนวนเต็ม port_width_sum(RTLIL::Cell *cell)
{
	unsigned จำนวนเต็ม sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	สำหรับ (auto param : port_width_params)
		ถ้า (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	คืน sum;
}

unsigned จำนวนเต็ม CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ถ้า (cmos_gate_cost().count(cell->type))
		คืน 1;

	ถ้า (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		คืน get(design_->module(cell->type));
	} มิฉะนั้น ถ้า (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		คืน cell->getParam(ID::WIDTH).as_int();
	} มิฉะนั้น ถ้า (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		คืน cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} มิฉะนั้น ถ้า (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		จำนวนเต็ม width = cell->getParam(param).as_int();
		ถ้า (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		คืน width * y_coef(cell->type);
	} มิฉะนั้น ถ้า (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned จำนวนเต็ม sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		คืน sum * sum_coef(cell->type);
	} มิฉะนั้น ถ้า (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned จำนวนเต็ม max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		คืน max * max_inp_coef(cell->type);
	} มิฉะนั้น ถ้า (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned จำนวนเต็ม sum = port_width_sum(cell);
		unsigned จำนวนเต็ม coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		คืน coef * sum * sum;
	} มิฉะนั้น ถ้า (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned จำนวนเต็ม cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned จำนวนเต็ม y_width = cell->getParam(ID::Y_WIDTH).as_int();
		สำหรับ (auto &term: macc.terms) {
			ถ้า (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned จำนวนเต็ม sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		คืน cost_sum;
	} มิฉะนั้น ถ้า (cell->type == ID($lut)) {
		จำนวนเต็ม width = cell->getParam(ID::WIDTH).as_int();
		unsigned จำนวนเต็ม cost = 1U << (unsigned จำนวนเต็ม)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		คืน cost;
	} มิฉะนั้น ถ้า (cell->type == ID($sop)) {
		จำนวนเต็ม width = cell->getParam(ID::WIDTH).as_int();
		จำนวนเต็ม depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		คืน (2 * width + 1) * depth;
	} มิฉะนั้น ถ้า (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		คืน 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	คืน 1;
}
