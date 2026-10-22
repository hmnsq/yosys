#अन्तर्भाव "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

नामाकाश Compressor_वृक्षः
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, पूर्णाङ्क विस्तारः)
{
	SigSpec योग = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
	SigSpec निर्गमः = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
	module->सङ्कलय_Fa(NEW_ID, a, b, c, निर्गमः, योग);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(निर्गमः.extract(0, विस्तारः - 1));
	प्रत्यागम {योग, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, पूर्णाङ्क विस्तारः)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
	SigSpec cout_h_पूर्णम् = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
	module->सङ्कलय_Fa(NEW_ID, a, b, c, cout_h_पूर्णम्, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec आगमः;
	आगमः.append(State::S0);
	यदि (विस्तारः > 1)
		आगमः.append(cout_h_पूर्णम्.extract(0, विस्तारः - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec योग = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
	SigSpec carry_पूर्णम् = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
	module->सङ्कलय_Fa(NEW_ID, s0, d, आगमः, carry_पूर्णम्, योग);

	SigSpec carry;
	carry.append(State::S0);
	यदि (विस्तारः > 1)
		carry.append(carry_पूर्णम्.extract(0, विस्तारः - 1));

	प्रत्यागम {योग, carry};
}

std::सदिशम्<गभीरता_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, तार्किक a_signed, तार्किक b_signed, पूर्णाङ्क विस्तारः) {
	पूर्णाङ्क विस्तारः_a = Get_आकार(a);
	पूर्णाङ्क विस्तारः_b = Get_आकार(b);
	std::सदिशम्<गभीरता_Sig> products;
	products.reserve(विस्तारः_a + 3);

	प्रति (पूर्णाङ्क i = 0; i < विस्तारः_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(विस्तारः, असत्य);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, विस्तारः);
		SigSpec row = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
		module->सङ्कलय_And(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		तार्किक row_is_bottom = (i == विस्तारः_a - 1);
		तार्किक any_inversion = (row_is_bottom && b_signed) || a_signed;

		यदि (any_inversion) {
			std::सदिशम्<RTLIL::State> आवरणम्(विस्तारः, RTLIL::State::S0);

			प्रति (पूर्णाङ्क j = 0; j < विस्तारः_b; j++) {
				पूर्णाङ्क col = i + j;
				यदि (col < 0 || col >= विस्तारः)
					अनुवर्त;
				तार्किक col_is_दक्षिणः = (j == विस्तारः_b - 1);
				// Flip masks
				तार्किक invert = (row_is_bottom && b_signed) ^ (col_is_दक्षिणः && a_signed);
				यदि (invert)
					आवरणम्[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			तार्किक nonzero = असत्य;
			प्रति (स्वचालित s : आवरणम्)
				यदि (s == RTLIL::State::S1) {
					nonzero = सत्य;
					भङ्ग;
				}
			यदि (nonzero) {
				SigSpec inverted = module->सङ्कलय_Wire(NEW_ID, विस्तारः);
				module->सङ्कलय_Xor(NEW_ID, row, SigSpec(RTLIL::Const(आवरणम्)), inverted);
				row = inverted;
			}
		}

		products.प्रक्षिप_पृष्ठभागः({row, 0});
	}

	// Correction constants
	स्वचालित प्रक्षिप_one_at = [&](पूर्णाङ्क col) {
		यदि (col < 0 || col >= विस्तारः)
			प्रत्यागम;
		std::सदिशम्<RTLIL::State> v(विस्तारः, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.प्रक्षिप_पृष्ठभागः({SigSpec(RTLIL::Const(v)), 0});
	};

	यदि (b_signed)
		प्रक्षिप_one_at(विस्तारः_a - 1);
	यदि (a_signed)
		प्रक्षिप_one_at(विस्तारः_b - 1);
	यदि (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		पूर्णाङ्क sign_extension_start = विस्तारः_a + विस्तारः_b - 1;
		यदि (sign_extension_start < विस्तारः) {
			std::सदिशम्<RTLIL::State> v(विस्तारः, RTLIL::State::S0);
			प्रति (पूर्णाङ्क i = sign_extension_start; i < विस्तारः; i++)
				v[i] = RTLIL::State::S1;
			products.प्रक्षिप_पृष्ठभागः({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	प्रत्यागम products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::सदिशम्<गभीरता_Sig> operands, पूर्णाङ्क विस्तारः, Strategy strategy, पूर्णाङ्क *बहिर्गमनम्_compressor_गणनम्, पूर्णाङ्क *बहिर्गमनम्_final_गभीरता) {
	पूर्णाङ्क स्तराः = 0;
	पूर्णाङ्क fa_गणनम् = 0;
	पूर्णाङ्क c42_गणनम् = 0;
	पूर्णाङ्क final_गभीरता = 0;

	प्रति (स्वचालित &op : operands)
		op.sig.extend_u0(विस्तारः);

	// Only compress operands ready at current level
	प्रति (पूर्णाङ्क level = 0; operands.आकार() > 2; level++) {
		// Partition operands into ready and waiting
		std::सदिशम्<गभीरता_Sig> ready;
		std::सदिशम्<गभीरता_Sig> प्रतीक्षा;
		ready.reserve(operands.आकार());
		प्रति (स्वचालित &op : operands) {
			यदि (op.गभीरता <= level)
				ready.प्रक्षिप_पृष्ठभागः(op);
			अन्यथा
				प्रतीक्षा.प्रक्षिप_पृष्ठभागः(op);
		}

		यदि (ready.आकार() < 3) {
			स्तराः++;
			अनुवर्त;
		}

		// Apply compressors to ready operands
		std::सदिशम्<गभीरता_Sig> compressed;
		compressed.reserve(ready.आकार());
		आकार_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		तार्किक try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		यावत् (i < ready.आकार()) {
			आकार_t remaining = ready.आकार() - i;

			यदि (try_42 && remaining >= 4) {
				गभीरता_Sig a = ready[i + 0];
				गभीरता_Sig b = ready[i + 1];
				गभीरता_Sig c = ready[i + 2];
				गभीरता_Sig d = ready[i + 3];

				स्वचालित [योग, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, विस्तारः);
				पूर्णाङ्क dmax = std::max({a.गभीरता, b.गभीरता, c.गभीरता, d.गभीरता});

				compressed.प्रक्षिप_पृष्ठभागः({योग, dmax + 2});
				compressed.प्रक्षिप_पृष्ठभागः({carry, dmax + 2});

				fa_गणनम् += 2;
				c42_गणनम् += 1;
				i += 4;
			} अन्यथा यदि (remaining >= 3) {
				गभीरता_Sig a = ready[i + 0];
				गभीरता_Sig b = ready[i + 1];
				गभीरता_Sig c = ready[i + 2];

				स्वचालित [योग, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, विस्तारः);
				पूर्णाङ्क dmax = std::max({a.गभीरता, b.गभीरता, c.गभीरता});

				compressed.प्रक्षिप_पृष्ठभागः({योग, dmax + 1});
				compressed.प्रक्षिप_पृष्ठभागः({carry, dmax + 1});

				fa_गणनम् += 1;
				i += 3;
			} अन्यथा {
				// Uncompressed operands pass through to next level
				प्रति (; i < ready.आकार(); i++)
					compressed.प्रक्षिप_पृष्ठभागः(ready[i]);
				भङ्ग;
			}
		}

		// Merge compressed with waiting operands
		प्रति (स्वचालित &op : प्रतीक्षा)
			compressed.प्रक्षिप_पृष्ठभागः(op);

		operands = std::move(compressed);
		स्तराः++;
	}

	यदि(बहिर्गमनम्_compressor_गणनम्)
		*बहिर्गमनम्_compressor_गणनम् = fa_गणनम्;
	यदि (operands.आकार() == 0) {
		यदि (बहिर्गमनम्_final_गभीरता)
			*बहिर्गमनम्_final_गभीरता = 0;
		प्रत्यागम {SigSpec(State::S0, विस्तारः), SigSpec(State::S0, विस्तारः)};
	}
	यदि (operands.आकार() == 1) {
		यदि (बहिर्गमनम्_final_गभीरता)
			*बहिर्गमनम्_final_गभीरता = operands[0].गभीरता;
		प्रत्यागम {operands[0].sig, SigSpec(State::S0, विस्तारः)};
	}

	final_गभीरता = std::max(operands[0].गभीरता, operands[1].गभीरता);
	यदि (बहिर्गमनम्_final_गभीरता)
		*बहिर्गमनम्_final_गभीरता = final_गभीरता;
	log_assert(operands.आकार() == 2);
	लघुगणकः("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", स्तराः, fa_गणनम्, c42_गणनम्, final_गभीरता);
	प्रत्यागम {operands[0].sig, operands[1].sig};
}

शून्य emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	पूर्णाङ्क विस्तारः = Get_आकार(y);
	log_assert(Get_आकार(a) == विस्तारः);
	log_assert(Get_आकार(b) == विस्तारः);

	यदि (विस्तारः == 0)
		प्रत्यागम;

	यदि (विस्तारः == 1) {
		module->सङ्कलय_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		प्रत्यागम;
	}

	// Bit level gen and prop
	std::सदिशम्<SigBit> g_pre(विस्तारः), p_pre(विस्तारः);
	प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++) {
		SigBit gi = module->सङ्कलय_Wire(NEW_ID);
		SigBit pi = module->सङ्कलय_Wire(NEW_ID);
		module->सङ्कलय_And_Gate(NEW_ID, a[i], b[i], gi);
		module->सङ्कलय_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::सदिशम्<SigBit> g = g_pre;
	std::सदिशम्<SigBit> p = p_pre;
	पूर्णाङ्क अङ्कः_स्तराः = 0;

	यावत् ((1 << अङ्कः_स्तराः) < विस्तारः)
		अङ्कः_स्तराः++;

	प्रति (पूर्णाङ्क k = 1; k <= अङ्कः_स्तराः; k++) {
		पूर्णाङ्क s = 1 << (k - 1);
		std::सदिशम्<SigBit> g_अग्र(विस्तारः), p_अग्र(विस्तारः);
		प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++) {
			यदि (i < s) {
				// Nothing to do
				g_अग्र[i] = g[i];
				p_अग्र[i] = p[i];
			} अन्यथा {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->सङ्कलय_Wire(NEW_ID);
				module->सङ्कलय_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->सङ्कलय_Wire(NEW_ID);
				module->सङ्कलय_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_अग्र[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				यदि (k < अङ्कः_स्तराः) {
					SigBit pnew = module->सङ्कलय_Wire(NEW_ID);
					module->सङ्कलय_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_अग्र[i] = pnew;
				} अन्यथा {
					// Skip last level
					p_अग्र[i] = State::Sx;
				}
			}
		}

		g = std::move(g_अग्र);
		p = std::move(p_अग्र);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	प्रति (पूर्णाङ्क i = 1; i < विस्तारः; i++)
		module->सङ्कलय_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	विकल्प (choice) {
		पक्ष FinalAdder::DEFAULT:
		पक्ष FinalAdder::RIPPLE: {
			प्रत्यागम module->सङ्कलय_सङ्कलय(NEW_ID, a, b, y, असत्य);
		}
		पक्ष FinalAdder::PARALLEL_उपसर्गः: {
			emit_kogge_stone(module, a, b, y);
			प्रत्यागम शून्यसूचकः;
		}
	}
	log_assert(असत्य && "CompressorTree::emit_final_adder: invalid choice");
	प्रत्यागम शून्यसूचकः;
}

FinalAdder pick_final_adder(पूर्णाङ्क विस्तारः, पूर्णाङ्क final_गभीरता, Final_बहुलकम् बहुलकम्) {
	विकल्प (बहुलकम्) {
		पक्ष Final_बहुलकम्::RIPPLE:  प्रत्यागम FinalAdder::RIPPLE;
		पक्ष Final_बहुलकम्::उपसर्गः:  प्रत्यागम FinalAdder::PARALLEL_उपसर्गः;
		पक्ष Final_बहुलकम्::AUTO:
		स्वभाव: {
			तार्किक wide = विस्तारः >= RIPPLE_उपसर्गः_विस्तारः_देहली;
			तार्किक deep = final_गभीरता >= RIPPLE_उपसर्गः_गभीरता_देहली;
			प्रत्यागम (wide && deep) ? FinalAdder::PARALLEL_उपसर्गः : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_अन्तः
