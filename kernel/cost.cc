#அந்தர்பாவ "kernel/cost.h"
#அந்தர்பாவ "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned முழு CellCosts::get(RTLIL::Module *mod)
{
	எனில் (mod_cost_cache_.மொத்தம்(mod->name))
		திரும்பு mod_cost_cache_.at(mod->name);

	unsigned முழு module_cost = 1;
	க்கு (auto c : mod->cells()) {
		unsigned முழு new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	திரும்பு module_cost;
}

நிலையான unsigned முழு y_coef(RTLIL::IdString type)
{
	எனில் (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		திரும்பு 1;
	} இல்லையெனில் எனில் (type == ID($neg)) {
		திரும்பு 4;
	} இல்லையெனில் எனில் (type == ID($demux)) {
		திரும்பு 2;
	} இல்லையெனில் எனில் (type == ID($fa)) {
		திரும்பு 5;
	} இல்லையெனில் எனில் (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		திரும்பு 8;
	} இல்லையெனில் எனில் (type.in(ID($shl), ID($sshl))) {
		// left shift
		திரும்பு 10;
	}
	திரும்பு 0;
}

நிலையான unsigned முழு max_inp_coef(RTLIL::IdString type)
{
	எனில் (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		திரும்பு 1;
	} இல்லையெனில் எனில் (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		திரும்பு 2;
	} இல்லையெனில் எனில் (type == ID($lcu)) {
		திரும்பு 5;
	} இல்லையெனில் எனில் (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		திரும்பு 7;
	}
	திரும்பு 0;
}

நிலையான unsigned முழு sum_coef(RTLIL::IdString type)
{
	எனில் (type.in(ID($shr), ID($sshr))) {
		// right shift
		திரும்பு 4;
	} இல்லையெனில் எனில் (type.in(ID($shift), ID($shiftx))) {
		// shift
		திரும்பு 8;
	}
	திரும்பு 0;
}

நிலையான unsigned முழு is_div_mod(RTLIL::IdString type)
{
	திரும்பு (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

நிலையான தருக்க is_free(RTLIL::IdString type)
{
	திரும்பு (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($ப்ரதிஜ்ஞா), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned முழு max_inp_width(RTLIL::Cell *cell)
{
	unsigned முழு max = 0;
	RTLIL::IdString உள்ளீடு_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	எனில் (cell->type == ID($bmux))
		திரும்பு cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	க்கு (RTLIL::IdString param : உள்ளீடு_width_params)
		எனில் (cell->hasParam(param))
			max = std::max(max, (unsigned முழு)cell->getParam(param).as_int());
	திரும்பு max;
}

unsigned முழு port_width_sum(RTLIL::Cell *cell)
{
	unsigned முழு sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	க்கு (auto param : port_width_params)
		எனில் (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	திரும்பு sum;
}

unsigned முழு CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	எனில் (cmos_gate_cost().மொத்தம்(cell->type))
		திரும்பு 1;

	எனில் (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		திரும்பு get(design_->module(cell->type));
	} இல்லையெனில் எனில் (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		திரும்பு cell->getParam(ID::WIDTH).as_int();
	} இல்லையெனில் எனில் (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		திரும்பு cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} இல்லையெனில் எனில் (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		முழு width = cell->getParam(param).as_int();
		எனில் (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		திரும்பு width * y_coef(cell->type);
	} இல்லையெனில் எனில் (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned முழு sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		திரும்பு sum * sum_coef(cell->type);
	} இல்லையெனில் எனில் (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned முழு max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		திரும்பு max * max_inp_coef(cell->type);
	} இல்லையெனில் எனில் (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned முழு sum = port_width_sum(cell);
		unsigned முழு coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		திரும்பு coef * sum * sum;
	} இல்லையெனில் எனில் (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned முழு cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned முழு y_width = cell->getParam(ID::Y_WIDTH).as_int();
		க்கு (auto &term: macc.terms) {
			எனில் (term.in_b.empty()) {
				// neglect addends
				தொடர்;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned முழு sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		திரும்பு cost_sum;
	} இல்லையெனில் எனில் (cell->type == ID($lut)) {
		முழு width = cell->getParam(ID::WIDTH).as_int();
		unsigned முழு cost = 1U << (unsigned முழு)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		திரும்பு cost;
	} இல்லையெனில் எனில் (cell->type == ID($sop)) {
		முழு width = cell->getParam(ID::WIDTH).as_int();
		முழு depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		திரும்பு (2 * width + 1) * depth;
	} இல்லையெனில் எனில் (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		திரும்பு 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	திரும்பு 1;
}
