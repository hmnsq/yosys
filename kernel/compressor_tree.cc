#అన్తర్భావ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

నామాకాశ Compressor_వృక్షం
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, పూర్ణాంకం వెడల్పు)
{
	SigSpec కూడిక = module->కూడు_Wire(NEW_ID, వెడల్పు);
	SigSpec బయటకు = module->కూడు_Wire(NEW_ID, వెడల్పు);
	module->కూడు_Fa(NEW_ID, a, b, c, బయటకు, కూడిక);

	SigSpec వాహకం;
	వాహకం.append(State::S0);
	వాహకం.append(బయటకు.వెలికితీయి(0, వెడల్పు - 1));
	తిరిగి {కూడిక, వాహకం};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, పూర్ణాంకం వెడల్పు)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->కూడు_Wire(NEW_ID, వెడల్పు);
	SigSpec cout_h_నిండిన = module->కూడు_Wire(NEW_ID, వెడల్పు);
	module->కూడు_Fa(NEW_ID, a, b, c, cout_h_నిండిన, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec లోపలికి;
	లోపలికి.append(State::S0);
	ఒకవేళ (వెడల్పు > 1)
		లోపలికి.append(cout_h_నిండిన.వెలికితీయి(0, వెడల్పు - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec కూడిక = module->కూడు_Wire(NEW_ID, వెడల్పు);
	SigSpec వాహకం_నిండిన = module->కూడు_Wire(NEW_ID, వెడల్పు);
	module->కూడు_Fa(NEW_ID, s0, d, లోపలికి, వాహకం_నిండిన, కూడిక);

	SigSpec వాహకం;
	వాహకం.append(State::S0);
	ఒకవేళ (వెడల్పు > 1)
		వాహకం.append(వాహకం_నిండిన.వెలికితీయి(0, వెడల్పు - 1));

	తిరిగి {కూడిక, వాహకం};
}

std::సదిశ<లోతు_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, తార్కిక a_signed, తార్కిక b_signed, పూర్ణాంకం వెడల్పు) {
	పూర్ణాంకం వెడల్పు_a = Get_కొలత(a);
	పూర్ణాంకం వెడల్పు_b = Get_కొలత(b);
	std::సదిశ<లోతు_Sig> products;
	products.reserve(వెడల్పు_a + 3);

	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు_a; i++) {
		Sig_బిట్టు ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(వెడల్పు, అబద్ధం);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, వెడల్పు);
		SigSpec అడ్డువరుస = module->కూడు_Wire(NEW_ID, వెడల్పు);
		module->కూడు_And(NEW_ID, b_shifted, ai_rep, అడ్డువరుస);

		// Apply Modified Baugh-Wooley inversions for this row
		తార్కిక అడ్డువరుస_is_bottom = (i == వెడల్పు_a - 1);
		తార్కిక any_inversion = (అడ్డువరుస_is_bottom && b_signed) || a_signed;

		ఒకవేళ (any_inversion) {
			std::సదిశ<RTLIL::State> ముసుగు(వెడల్పు, RTLIL::State::S0);

			కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు_b; j++) {
				పూర్ణాంకం నిలువు = i + j;
				ఒకవేళ (నిలువు < 0 || నిలువు >= వెడల్పు)
					కొనసాగు;
				తార్కిక నిలువు_is_కుడి = (j == వెడల్పు_b - 1);
				// Flip masks
				తార్కిక invert = (అడ్డువరుస_is_bottom && b_signed) ^ (నిలువు_is_కుడి && a_signed);
				ఒకవేళ (invert)
					ముసుగు[నిలువు] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			తార్కిక nonzero = అబద్ధం;
			కొరకు (స్వయంచాలక s : ముసుగు)
				ఒకవేళ (s == RTLIL::State::S1) {
					nonzero = నిజం;
					ఆపు;
				}
			ఒకవేళ (nonzero) {
				SigSpec inverted = module->కూడు_Wire(NEW_ID, వెడల్పు);
				module->కూడు_Xor(NEW_ID, అడ్డువరుస, SigSpec(RTLIL::Const(ముసుగు)), inverted);
				అడ్డువరుస = inverted;
			}
		}

		products.నెట్టు_వెనుక({అడ్డువరుస, 0});
	}

	// Correction constants
	స్వయంచాలక నెట్టు_one_at = [&](పూర్ణాంకం నిలువు) {
		ఒకవేళ (నిలువు < 0 || నిలువు >= వెడల్పు)
			తిరిగి;
		std::సదిశ<RTLIL::State> v(వెడల్పు, RTLIL::State::S0);
		v[నిలువు] = RTLIL::State::S1;
		products.నెట్టు_వెనుక({SigSpec(RTLIL::Const(v)), 0});
	};

	ఒకవేళ (b_signed)
		నెట్టు_one_at(వెడల్పు_a - 1);
	ఒకవేళ (a_signed)
		నెట్టు_one_at(వెడల్పు_b - 1);
	ఒకవేళ (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		పూర్ణాంకం sign_extension_ప్రారంభం = వెడల్పు_a + వెడల్పు_b - 1;
		ఒకవేళ (sign_extension_ప్రారంభం < వెడల్పు) {
			std::సదిశ<RTLIL::State> v(వెడల్పు, RTLIL::State::S0);
			కొరకు (పూర్ణాంకం i = sign_extension_ప్రారంభం; i < వెడల్పు; i++)
				v[i] = RTLIL::State::S1;
			products.నెట్టు_వెనుక({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	తిరిగి products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::సదిశ<లోతు_Sig> operands, పూర్ణాంకం వెడల్పు, Strategy strategy, పూర్ణాంకం *బహిర్గతం_compressor_లెక్క, పూర్ణాంకం *బహిర్గతం_final_లోతు) {
	పూర్ణాంకం స్థాయిలు = 0;
	పూర్ణాంకం fa_లెక్క = 0;
	పూర్ణాంకం c42_లెక్క = 0;
	పూర్ణాంకం final_లోతు = 0;

	కొరకు (స్వయంచాలక &op : operands)
		op.sig.extend_u0(వెడల్పు);

	// Only compress operands ready at current level
	కొరకు (పూర్ణాంకం level = 0; operands.కొలత() > 2; level++) {
		// Partition operands into ready and waiting
		std::సదిశ<లోతు_Sig> ready;
		std::సదిశ<లోతు_Sig> నిరీక్షణ;
		ready.reserve(operands.కొలత());
		కొరకు (స్వయంచాలక &op : operands) {
			ఒకవేళ (op.లోతు <= level)
				ready.నెట్టు_వెనుక(op);
			లేకపోతే
				నిరీక్షణ.నెట్టు_వెనుక(op);
		}

		ఒకవేళ (ready.కొలత() < 3) {
			స్థాయిలు++;
			కొనసాగు;
		}

		// Apply compressors to ready operands
		std::సదిశ<లోతు_Sig> compressed;
		compressed.reserve(ready.కొలత());
		కొలత_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		తార్కిక try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		అయితే (i < ready.కొలత()) {
			కొలత_t మిగిలిన = ready.కొలత() - i;

			ఒకవేళ (try_42 && మిగిలిన >= 4) {
				లోతు_Sig a = ready[i + 0];
				లోతు_Sig b = ready[i + 1];
				లోతు_Sig c = ready[i + 2];
				లోతు_Sig d = ready[i + 3];

				స్వయంచాలక [కూడిక, వాహకం] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, వెడల్పు);
				పూర్ణాంకం dmax = std::max({a.లోతు, b.లోతు, c.లోతు, d.లోతు});

				compressed.నెట్టు_వెనుక({కూడిక, dmax + 2});
				compressed.నెట్టు_వెనుక({వాహకం, dmax + 2});

				fa_లెక్క += 2;
				c42_లెక్క += 1;
				i += 4;
			} లేకపోతే ఒకవేళ (మిగిలిన >= 3) {
				లోతు_Sig a = ready[i + 0];
				లోతు_Sig b = ready[i + 1];
				లోతు_Sig c = ready[i + 2];

				స్వయంచాలక [కూడిక, వాహకం] = emit_compressor_32(module, a.sig, b.sig, c.sig, వెడల్పు);
				పూర్ణాంకం dmax = std::max({a.లోతు, b.లోతు, c.లోతు});

				compressed.నెట్టు_వెనుక({కూడిక, dmax + 1});
				compressed.నెట్టు_వెనుక({వాహకం, dmax + 1});

				fa_లెక్క += 1;
				i += 3;
			} లేకపోతే {
				// Uncompressed operands pass through to next level
				కొరకు (; i < ready.కొలత(); i++)
					compressed.నెట్టు_వెనుక(ready[i]);
				ఆపు;
			}
		}

		// Merge compressed with waiting operands
		కొరకు (స్వయంచాలక &op : నిరీక్షణ)
			compressed.నెట్టు_వెనుక(op);

		operands = std::move(compressed);
		స్థాయిలు++;
	}

	ఒకవేళ(బహిర్గతం_compressor_లెక్క)
		*బహిర్గతం_compressor_లెక్క = fa_లెక్క;
	ఒకవేళ (operands.కొలత() == 0) {
		ఒకవేళ (బహిర్గతం_final_లోతు)
			*బహిర్గతం_final_లోతు = 0;
		తిరిగి {SigSpec(State::S0, వెడల్పు), SigSpec(State::S0, వెడల్పు)};
	}
	ఒకవేళ (operands.కొలత() == 1) {
		ఒకవేళ (బహిర్గతం_final_లోతు)
			*బహిర్గతం_final_లోతు = operands[0].లోతు;
		తిరిగి {operands[0].sig, SigSpec(State::S0, వెడల్పు)};
	}

	final_లోతు = std::max(operands[0].లోతు, operands[1].లోతు);
	ఒకవేళ (బహిర్గతం_final_లోతు)
		*బహిర్గతం_final_లోతు = final_లోతు;
	log_assert(operands.కొలత() == 2);
	సంవర్గమానం("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", స్థాయిలు, fa_లెక్క, c42_లెక్క, final_లోతు);
	తిరిగి {operands[0].sig, operands[1].sig};
}

శూన్య emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	పూర్ణాంకం వెడల్పు = Get_కొలత(y);
	log_assert(Get_కొలత(a) == వెడల్పు);
	log_assert(Get_కొలత(b) == వెడల్పు);

	ఒకవేళ (వెడల్పు == 0)
		తిరిగి;

	ఒకవేళ (వెడల్పు == 1) {
		module->కూడు_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		తిరిగి;
	}

	// Bit level gen and prop
	std::సదిశ<Sig_బిట్టు> g_pre(వెడల్పు), p_pre(వెడల్పు);
	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++) {
		Sig_బిట్టు gi = module->కూడు_Wire(NEW_ID);
		Sig_బిట్టు pi = module->కూడు_Wire(NEW_ID);
		module->కూడు_And_Gate(NEW_ID, a[i], b[i], gi);
		module->కూడు_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::సదిశ<Sig_బిట్టు> g = g_pre;
	std::సదిశ<Sig_బిట్టు> p = p_pre;
	పూర్ణాంకం అంకె_స్థాయిలు = 0;

	అయితే ((1 << అంకె_స్థాయిలు) < వెడల్పు)
		అంకె_స్థాయిలు++;

	కొరకు (పూర్ణాంకం k = 1; k <= అంకె_స్థాయిలు; k++) {
		పూర్ణాంకం s = 1 << (k - 1);
		std::సదిశ<Sig_బిట్టు> g_తదుపరి(వెడల్పు), p_తదుపరి(వెడల్పు);
		కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++) {
			ఒకవేళ (i < s) {
				// Nothing to do
				g_తదుపరి[i] = g[i];
				p_తదుపరి[i] = p[i];
			} లేకపోతే {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_బిట్టు and_pg = module->కూడు_Wire(NEW_ID);
				module->కూడు_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_బిట్టు gnew = module->కూడు_Wire(NEW_ID);
				module->కూడు_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_తదుపరి[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				ఒకవేళ (k < అంకె_స్థాయిలు) {
					Sig_బిట్టు pnew = module->కూడు_Wire(NEW_ID);
					module->కూడు_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_తదుపరి[i] = pnew;
				} లేకపోతే {
					// Skip last level
					p_తదుపరి[i] = State::Sx;
				}
			}
		}

		g = std::move(g_తదుపరి);
		p = std::move(p_తదుపరి);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	కొరకు (పూర్ణాంకం i = 1; i < వెడల్పు; i++)
		module->కూడు_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	ఎంపిక (choice) {
		స్థితి FinalAdder::DEFAULT:
		స్థితి FinalAdder::RIPPLE: {
			తిరిగి module->కూడు_కూడు(NEW_ID, a, b, y, అబద్ధం);
		}
		స్థితి FinalAdder::PARALLEL_ఉపసర్గ: {
			emit_kogge_stone(module, a, b, y);
			తిరిగి శూన్యసూచిక;
		}
	}
	log_assert(అబద్ధం && "CompressorTree::emit_final_adder: invalid choice");
	తిరిగి శూన్యసూచిక;
}

FinalAdder pick_final_adder(పూర్ణాంకం వెడల్పు, పూర్ణాంకం final_లోతు, Final_బహుళకం బహుళకం) {
	ఎంపిక (బహుళకం) {
		స్థితి Final_బహుళకం::RIPPLE:  తిరిగి FinalAdder::RIPPLE;
		స్థితి Final_బహుళకం::ఉపసర్గ:  తిరిగి FinalAdder::PARALLEL_ఉపసర్గ;
		స్థితి Final_బహుళకం::AUTO:
		అప్రమేయ: {
			తార్కిక wide = వెడల్పు >= RIPPLE_ఉపసర్గ_వెడల్పు_పరిమితి;
			తార్కిక deep = final_లోతు >= RIPPLE_ఉపసర్గ_లోతు_పరిమితి;
			తిరిగి (wide && deep) ? FinalAdder::PARALLEL_ఉపసర్గ : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_చివర
