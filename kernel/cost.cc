#అన్తర్భావ "kernel/cost.h"
#అన్తర్భావ "kernel/macc.h"

USING_YOSYS_NAMESPACE

గుర్తులేని పూర్ణాంకం CellCosts::get(RTLIL::Module *mod)
{
	ఒకవేళ (mod_ఖర్చు_cache.లెక్క(mod->name))
		తిరిగి mod_ఖర్చు_cache.at(mod->name);

	గుర్తులేని పూర్ణాంకం module_ఖర్చు = 1;
	కొరకు (స్వయంచాలక c : mod->cells()) {
		గుర్తులేని పూర్ణాంకం new_ఖర్చు = module_ఖర్చు + get(c);
		module_ఖర్చు = new_ఖర్చు >= module_ఖర్చు ? new_ఖర్చు : గరిష్ఠ_పూర్ణాంకం;
	}

	mod_ఖర్చు_cache[mod->name] = module_ఖర్చు;
	తిరిగి module_ఖర్చు;
}

స్థిర గుర్తులేని పూర్ణాంకం y_coef(RTLIL::IdString type)
{
	ఒకవేళ (
	  // equality
	  type.లోపల(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.లోపల(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.లోపల(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		తిరిగి 1;
	} లేకపోతే ఒకవేళ (type == ID($neg)) {
		తిరిగి 4;
	} లేకపోతే ఒకవేళ (type == ID($demux)) {
		తిరిగి 2;
	} లేకపోతే ఒకవేళ (type == ID($fa)) {
		తిరిగి 5;
	} లేకపోతే ఒకవేళ (type.లోపల(ID($కూడు), ID($sub), ID($alu))) {
		// multi-bit adders
		తిరిగి 8;
	} లేకపోతే ఒకవేళ (type.లోపల(ID($shl), ID($sshl))) {
		// left shift
		తిరిగి 10;
	}
	తిరిగి 0;
}

స్థిర గుర్తులేని పూర్ణాంకం max_inp_coef(RTLIL::IdString type)
{
	ఒకవేళ (
	  // binop reduce
	  type.లోపల(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.లోపల(ID($logic_not), ID($pmux), ID($bmux))) {
		తిరిగి 1;
	} లేకపోతే ఒకవేళ (
	  // equality
	  type.లోపల(ID($eq), ID($ne)) ||
	  // logic
	  type.లోపల(ID($logic_and), ID($logic_or))) {
		తిరిగి 2;
	} లేకపోతే ఒకవేళ (type == ID($lcu)) {
		తిరిగి 5;
	} లేకపోతే ఒకవేళ (type.లోపల(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		తిరిగి 7;
	}
	తిరిగి 0;
}

స్థిర గుర్తులేని పూర్ణాంకం కూడిక_coef(RTLIL::IdString type)
{
	ఒకవేళ (type.లోపల(ID($shr), ID($sshr))) {
		// right shift
		తిరిగి 4;
	} లేకపోతే ఒకవేళ (type.లోపల(ID($జరుపు), ID($shiftx))) {
		//జరుపు
		తిరిగి 8;
	}
	తిరిగి 0;
}

స్థిర గుర్తులేని పూర్ణాంకం is_div_mod(RTLIL::IdString type)
{
	తిరిగి (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

స్థిర తార్కిక is_free(RTLIL::IdString type)
{
	తిరిగి (
	  //ట్యాగులు
	  type.లోపల(ID($overwrite_ట్యాగ్), ID($సమితి_ట్యాగ్), ID($original_ట్యాగ్), ID($get_ట్యాగ్)) ||
	  // formal
	  type.లోపల(ID($check), ID($equiv), ID($initstate), ID($ప్రతిజ్ఞా), ID($assume), ID($సజీవ), ID($cover), ID($fair)) ||
	  type.లోపల(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.లోపల(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.లోపల(ID($concat), ID($ముక్క), ID($pos)) ||
	  // specify
	  type.లోపల(ID($specrule), ID($specify2), ID($specify3)));
}

గుర్తులేని పూర్ణాంకం max_inp_వెడల్పు(RTLIL::Cell *cell)
{
	గుర్తులేని పూర్ణాంకం max = 0;
	RTLIL::IdString ఆదానం_వెడల్పు_params[] = {
	  ID::వెడల్పు_2,
	  ID::A_వెడల్పు,
	  ID::B_వెడల్పు,
	  ID::S_వెడల్పు,
	};

	ఒకవేళ (cell->type == ID($bmux))
		తిరిగి cell->getParam(ID::వెడల్పు_2).as_int() << cell->getParam(ID::S_వెడల్పు).as_int();

	కొరకు (RTLIL::IdString param : ఆదానం_వెడల్పు_params)
		ఒకవేళ (cell->hasParam(param))
			max = std::max(max, (గుర్తులేని పూర్ణాంకం)cell->getParam(param).as_int());
	తిరిగి max;
}

గుర్తులేని పూర్ణాంకం పోర్టు_వెడల్పు_కూడిక(RTLIL::Cell *cell)
{
	గుర్తులేని పూర్ణాంకం కూడిక = 0;
	RTLIL::IdString పోర్టు_వెడల్పు_params[] = {
	  ID::వెడల్పు_2, ID::A_వెడల్పు, ID::B_వెడల్పు, ID::S_వెడల్పు, ID::Y_వెడల్పు,
	};

	కొరకు (స్వయంచాలక param : పోర్టు_వెడల్పు_params)
		ఒకవేళ (cell->hasParam(param))
			కూడిక += cell->getParam(param).as_int();

	తిరిగి కూడిక;
}

గుర్తులేని పూర్ణాంకం CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ఒకవేళ (cmos_gate_ఖర్చు().లెక్క(cell->type))
		తిరిగి 1;

	ఒకవేళ (design_ && design_->module(cell->type) && cell->parameters.ఖాళీలు()) {
		log_debug("%s is a module, recurse\n", cell->name);
		తిరిగి get(design_->module(cell->type));
	} లేకపోతే ఒకవేళ (cell->is_builtin_ff()) {
		log_assert(cell->has_పోర్టు(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		తిరిగి cell->getParam(ID::వెడల్పు_2).as_int();
	} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		తిరిగి cell->getParam(ID::వెడల్పు_2).as_int() * cell->getParam(ID::కొలత_2).as_int();
	} లేకపోతే ఒకవేళ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_వెడల్పు) || cell->hasParam(ID::వెడల్పు_2)) && "Unknown width");
		స్వయంచాలక param = cell->hasParam(ID::Y_వెడల్పు) ? ID::Y_వెడల్పు : ID::వెడల్పు_2;
		పూర్ణాంకం వెడల్పు = cell->getParam(param).as_int();
		ఒకవేళ (cell->type == ID($demux))
			వెడల్పు <<= cell->getParam(ID::S_వెడల్పు).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, వెడల్పు, y_coef(cell->type));
		తిరిగి వెడల్పు * y_coef(cell->type);
	} లేకపోతే ఒకవేళ (కూడిక_coef(cell->type)) {
		// linear with sum of port widths
		గుర్తులేని పూర్ణాంకం కూడిక = పోర్టు_వెడల్పు_కూడిక(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, కూడిక, కూడిక_coef(cell->type));
		తిరిగి కూడిక * కూడిక_coef(cell->type);
	} లేకపోతే ఒకవేళ (max_inp_coef(cell->type)) {
		// linear with largest input width
		గుర్తులేని పూర్ణాంకం max = max_inp_వెడల్పు(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		తిరిగి max * max_inp_coef(cell->type);
	} లేకపోతే ఒకవేళ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		గుర్తులేని పూర్ణాంకం కూడిక = పోర్టు_వెడల్పు_కూడిక(cell);
		గుర్తులేని పూర్ణాంకం coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, కూడిక * కూడిక);
		తిరిగి coef * కూడిక * కూడిక;
	} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($macc), ID($macc_v2))) {
		// quadratic per term
		గుర్తులేని పూర్ణాంకం ఖర్చు_కూడిక = 0;
		Macc macc;
		macc.from_cell(cell);
		గుర్తులేని పూర్ణాంకం y_వెడల్పు = cell->getParam(ID::Y_వెడల్పు).as_int();
		కొరకు (స్వయంచాలక &పదం: macc.terms) {
			ఒకవేళ (పదం.లోపల_b.ఖాళీలు()) {
				// neglect addends
				కొనసాగు;
			}
			గుర్తులేని a_వెడల్పు = పదం.లోపల_a.కొలత(), b_వెడల్పు = పదం.లోపల_b.కొలత();
			గుర్తులేని పూర్ణాంకం కూడిక = a_వెడల్పు + b_వెడల్పు + std::min(y_వెడల్పు, a_వెడల్పు + b_వెడల్పు);
			ఖర్చు_కూడిక += 3 * కూడిక * కూడిక;
		}
		తిరిగి ఖర్చు_కూడిక;
	} లేకపోతే ఒకవేళ (cell->type == ID($lut)) {
		పూర్ణాంకం వెడల్పు = cell->getParam(ID::వెడల్పు_2).as_int();
		గుర్తులేని పూర్ణాంకం ఖర్చు = 1U << (గుర్తులేని పూర్ణాంకం)వెడల్పు;
		log_debug("%s is 2**%d\n", cell->name, వెడల్పు);
		తిరిగి ఖర్చు;
	} లేకపోతే ఒకవేళ (cell->type == ID($sop)) {
		పూర్ణాంకం వెడల్పు = cell->getParam(ID::వెడల్పు_2).as_int();
		పూర్ణాంకం లోతు = cell->getParam(ID::లోతు_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, వెడల్పు, లోతు);
		తిరిగి (2 * వెడల్పు + 1) * లోతు;
	} లేకపోతే ఒకవేళ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		తిరిగి 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_కొలత(cell->parameters));
	తిరిగి 1;
}
