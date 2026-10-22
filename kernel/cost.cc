#ಅನ್ತರ್ಭಾವ "kernel/cost.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"

USING_YOSYS_NAMESPACE

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ CellCosts::get(RTLIL::Module *mod)
{
	ಆದರೆ (mod_ವೆಚ್ಚ_cache.ಎಣಿಕೆ_2(mod->name))
		ಹಿಂತಿರುಗಿ mod_ವೆಚ್ಚ_cache.at(mod->name);

	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ module_ವೆಚ್ಚ = 1;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c : mod->cells()) {
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ new_ವೆಚ್ಚ = module_ವೆಚ್ಚ + get(c);
		module_ವೆಚ್ಚ = new_ವೆಚ್ಚ >= module_ವೆಚ್ಚ ? new_ವೆಚ್ಚ : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
	}

	mod_ವೆಚ್ಚ_cache[mod->name] = module_ವೆಚ್ಚ;
	ಹಿಂತಿರುಗಿ module_ವೆಚ್ಚ;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ y_coef(RTLIL::IdString type)
{
	ಆದರೆ (
	  // equality
	  type.ಒಳ(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.ಒಳ(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.ಒಳ(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ಹಿಂತಿರುಗಿ 1;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ID($neg)) {
		ಹಿಂತಿರುಗಿ 4;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ID($demux)) {
		ಹಿಂತಿರುಗಿ 2;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ID($fa)) {
		ಹಿಂತಿರುಗಿ 5;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type.ಒಳ(ID($ಕೂಡಿಸು), ID($sub), ID($alu))) {
		// multi-bit adders
		ಹಿಂತಿರುಗಿ 8;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type.ಒಳ(ID($shl), ID($sshl))) {
		// left shift
		ಹಿಂತಿರುಗಿ 10;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_inp_coef(RTLIL::IdString type)
{
	ಆದರೆ (
	  // binop reduce
	  type.ಒಳ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.ಒಳ(ID($logic_not), ID($pmux), ID($bmux))) {
		ಹಿಂತಿರುಗಿ 1;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (
	  // equality
	  type.ಒಳ(ID($eq), ID($ne)) ||
	  // logic
	  type.ಒಳ(ID($logic_and), ID($logic_or))) {
		ಹಿಂತಿರುಗಿ 2;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ID($lcu)) {
		ಹಿಂತಿರುಗಿ 5;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type.ಒಳ(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ಹಿಂತಿರುಗಿ 7;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೊತ್ತ_coef(RTLIL::IdString type)
{
	ಆದರೆ (type.ಒಳ(ID($shr), ID($sshr))) {
		// right shift
		ಹಿಂತಿರುಗಿ 4;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type.ಒಳ(ID($ಸರಿಸು), ID($shiftx))) {
		//ಸರಿಸು
		ಹಿಂತಿರುಗಿ 8;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ is_div_mod(RTLIL::IdString type)
{
	ಹಿಂತಿರುಗಿ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ is_free(RTLIL::IdString type)
{
	ಹಿಂತಿರುಗಿ (
	  //ಟ್ಯಾಗುಗಳು
	  type.ಒಳ(ID($overwrite_ಟ್ಯಾಗ್), ID($ಗಣ_ಟ್ಯಾಗ್), ID($original_ಟ್ಯಾಗ್), ID($get_ಟ್ಯಾಗ್)) ||
	  // formal
	  type.ಒಳ(ID($check), ID($equiv), ID($initstate), ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($ಜೀವಂತ), ID($cover), ID($fair)) ||
	  type.ಒಳ(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.ಒಳ(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.ಒಳ(ID($concat), ID($ತುಂಡು), ID($pos)) ||
	  // specify
	  type.ಒಳ(ID($specrule), ID($specify2), ID($specify3)));
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_inp_ಅಗಲ(RTLIL::Cell *cell)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max = 0;
	RTLIL::IdString ಒಳಹರಿವು_ಅಗಲ_params[] = {
	  ID::ಅಗಲ_2,
	  ID::A_ಅಗಲ,
	  ID::B_ಅಗಲ,
	  ID::S_ಅಗಲ,
	};

	ಆದರೆ (cell->type == ID($bmux))
		ಹಿಂತಿರುಗಿ cell->getParam(ID::ಅಗಲ_2).as_int() << cell->getParam(ID::S_ಅಗಲ).as_int();

	ಗಾಗಿ (RTLIL::IdString param : ಒಳಹರಿವು_ಅಗಲ_params)
		ಆದರೆ (cell->hasParam(param))
			max = std::max(max, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cell->getParam(param).as_int());
	ಹಿಂತಿರುಗಿ max;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್_ಅಗಲ_ಮೊತ್ತ(RTLIL::Cell *cell)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೊತ್ತ = 0;
	RTLIL::IdString ಪೋರ್ಟ್_ಅಗಲ_params[] = {
	  ID::ಅಗಲ_2, ID::A_ಅಗಲ, ID::B_ಅಗಲ, ID::S_ಅಗಲ, ID::Y_ಅಗಲ,
	};

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ param : ಪೋರ್ಟ್_ಅಗಲ_params)
		ಆದರೆ (cell->hasParam(param))
			ಮೊತ್ತ += cell->getParam(param).as_int();

	ಹಿಂತಿರುಗಿ ಮೊತ್ತ;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ಆದರೆ (cmos_gate_ವೆಚ್ಚ().ಎಣಿಕೆ_2(cell->type))
		ಹಿಂತಿರುಗಿ 1;

	ಆದರೆ (design_ && design_->module(cell->type) && cell->parameters.ಖಾಲಿಗಳು()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ಹಿಂತಿರುಗಿ get(design_->module(cell->type));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->is_builtin_ff()) {
		log_assert(cell->has_ಪೋರ್ಟ್(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ಹಿಂತಿರುಗಿ cell->getParam(ID::ಅಗಲ_2).as_int();
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ಹಿಂತಿರುಗಿ cell->getParam(ID::ಅಗಲ_2).as_int() * cell->getParam(ID::ಗಾತ್ರ_3).as_int();
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_ಅಗಲ) || cell->hasParam(ID::ಅಗಲ_2)) && "Unknown width");
		ಸ್ವಯಂಚಾಲಿತ param = cell->hasParam(ID::Y_ಅಗಲ) ? ID::Y_ಅಗಲ : ID::ಅಗಲ_2;
		ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->getParam(param).as_int();
		ಆದರೆ (cell->type == ID($demux))
			ಅಗಲ <<= cell->getParam(ID::S_ಅಗಲ).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, ಅಗಲ, y_coef(cell->type));
		ಹಿಂತಿರುಗಿ ಅಗಲ * y_coef(cell->type);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊತ್ತ_coef(cell->type)) {
		// linear with sum of port widths
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೊತ್ತ = ಪೋರ್ಟ್_ಅಗಲ_ಮೊತ್ತ(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, ಮೊತ್ತ, ಮೊತ್ತ_coef(cell->type));
		ಹಿಂತಿರುಗಿ ಮೊತ್ತ * ಮೊತ್ತ_coef(cell->type);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_inp_coef(cell->type)) {
		// linear with largest input width
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max = max_inp_ಅಗಲ(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ಹಿಂತಿರುಗಿ max * max_inp_coef(cell->type);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೊತ್ತ = ಪೋರ್ಟ್_ಅಗಲ_ಮೊತ್ತ(cell);
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, ಮೊತ್ತ * ಮೊತ್ತ);
		ಹಿಂತಿರುಗಿ coef * ಮೊತ್ತ * ಮೊತ್ತ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($macc), ID($macc_v2))) {
		// quadratic per term
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ವೆಚ್ಚ_ಮೊತ್ತ = 0;
		Macc macc;
		macc.from_cell(cell);
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ y_ಅಗಲ = cell->getParam(ID::Y_ಅಗಲ).as_int();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪದ: macc.terms) {
			ಆದರೆ (ಪದ.ಒಳ_b.ಖಾಲಿಗಳು()) {
				// neglect addends
				ಮುಂದುವರಿ;
			}
			ಚಿಹ್ನೆರಹಿತ a_ಅಗಲ = ಪದ.ಒಳ_a.ಗಾತ್ರ_2(), b_ಅಗಲ = ಪದ.ಒಳ_b.ಗಾತ್ರ_2();
			ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೊತ್ತ = a_ಅಗಲ + b_ಅಗಲ + std::min(y_ಅಗಲ, a_ಅಗಲ + b_ಅಗಲ);
			ವೆಚ್ಚ_ಮೊತ್ತ += 3 * ಮೊತ್ತ * ಮೊತ್ತ;
		}
		ಹಿಂತಿರುಗಿ ವೆಚ್ಚ_ಮೊತ್ತ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($lut)) {
		ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->getParam(ID::ಅಗಲ_2).as_int();
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ವೆಚ್ಚ = 1U << (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಗಲ;
		log_debug("%s is 2**%d\n", cell->name, ಅಗಲ);
		ಹಿಂತಿರುಗಿ ವೆಚ್ಚ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($sop)) {
		ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->getParam(ID::ಅಗಲ_2).as_int();
		ಪೂರ್ಣಾಂಕ ಆಳ = cell->getParam(ID::ಆಳ_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, ಅಗಲ, ಆಳ);
		ಹಿಂತಿರುಗಿ (2 * ಅಗಲ + 1) * ಆಳ;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ಹಿಂತಿರುಗಿ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_ಗಾತ್ರ(cell->parameters));
	ಹಿಂತಿರುಗಿ 1;
}
