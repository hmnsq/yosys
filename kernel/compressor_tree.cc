#ಅನ್ತರ್ಭಾವ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ Compressor_ವೃಕ್ಷ
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	SigSpec ಮೊತ್ತ = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	SigSpec ಹೊರಗೆ = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	module->ಕೂಡಿಸು_Fa(NEW_ID, a, b, c, ಹೊರಗೆ, ಮೊತ್ತ);

	SigSpec ವಾಹಕ;
	ವಾಹಕ.append(State::S0);
	ವಾಹಕ.append(ಹೊರಗೆ.ಹೊರತೆಗೆ(0, ಅಗಲ - 1));
	ಹಿಂತಿರುಗಿ {ಮೊತ್ತ, ವಾಹಕ};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	SigSpec cout_h_ತುಂಬಿದ = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	module->ಕೂಡಿಸು_Fa(NEW_ID, a, b, c, cout_h_ತುಂಬಿದ, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec ಒಳಗೆ;
	ಒಳಗೆ.append(State::S0);
	ಆದರೆ (ಅಗಲ > 1)
		ಒಳಗೆ.append(cout_h_ತುಂಬಿದ.ಹೊರತೆಗೆ(0, ಅಗಲ - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec ಮೊತ್ತ = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	SigSpec ವಾಹಕ_ತುಂಬಿದ = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	module->ಕೂಡಿಸು_Fa(NEW_ID, s0, d, ಒಳಗೆ, ವಾಹಕ_ತುಂಬಿದ, ಮೊತ್ತ);

	SigSpec ವಾಹಕ;
	ವಾಹಕ.append(State::S0);
	ಆದರೆ (ಅಗಲ > 1)
		ವಾಹಕ.append(ವಾಹಕ_ತುಂಬಿದ.ಹೊರತೆಗೆ(0, ಅಗಲ - 1));

	ಹಿಂತಿರುಗಿ {ಮೊತ್ತ, ವಾಹಕ};
}

std::ಸದಿಶ<ಆಳ_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, ತಾರ್ಕಿಕ a_signed, ತಾರ್ಕಿಕ b_signed, ಪೂರ್ಣಾಂಕ ಅಗಲ) {
	ಪೂರ್ಣಾಂಕ ಅಗಲ_a = Get_ಗಾತ್ರ(a);
	ಪೂರ್ಣಾಂಕ ಅಗಲ_b = Get_ಗಾತ್ರ(b);
	std::ಸದಿಶ<ಆಳ_Sig> products;
	products.reserve(ಅಗಲ_a + 3);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ_a; i++) {
		Sig_ಬಿಟ್ ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(ಅಗಲ, ಸುಳ್ಳು);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, ಅಗಲ);
		SigSpec ಅಡ್ಡಸಾಲು = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
		module->ಕೂಡಿಸು_And(NEW_ID, b_shifted, ai_rep, ಅಡ್ಡಸಾಲು);

		// Apply Modified Baugh-Wooley inversions for this row
		ತಾರ್ಕಿಕ ಅಡ್ಡಸಾಲು_is_bottom = (i == ಅಗಲ_a - 1);
		ತಾರ್ಕಿಕ any_inversion = (ಅಡ್ಡಸಾಲು_is_bottom && b_signed) || a_signed;

		ಆದರೆ (any_inversion) {
			std::ಸದಿಶ<RTLIL::State> ಮುಖವಾಡ(ಅಗಲ, RTLIL::State::S0);

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ_b; j++) {
				ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = i + j;
				ಆದರೆ (ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= ಅಗಲ)
					ಮುಂದುವರಿ;
				ತಾರ್ಕಿಕ ಲಂಬಸಾಲು_is_ಬಲ = (j == ಅಗಲ_b - 1);
				// Flip masks
				ತಾರ್ಕಿಕ invert = (ಅಡ್ಡಸಾಲು_is_bottom && b_signed) ^ (ಲಂಬಸಾಲು_is_ಬಲ && a_signed);
				ಆದರೆ (invert)
					ಮುಖವಾಡ[ಲಂಬಸಾಲು] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			ತಾರ್ಕಿಕ nonzero = ಸುಳ್ಳು;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ s : ಮುಖವಾಡ)
				ಆದರೆ (s == RTLIL::State::S1) {
					nonzero = ನಿಜ;
					ನಿಲ್ಲಿಸು;
				}
			ಆದರೆ (nonzero) {
				SigSpec inverted = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
				module->ಕೂಡಿಸು_Xor(NEW_ID, ಅಡ್ಡಸಾಲು, SigSpec(RTLIL::Const(ಮುಖವಾಡ)), inverted);
				ಅಡ್ಡಸಾಲು = inverted;
			}
		}

		products.ತಳ್ಳು_ಹಿಂಭಾಗ({ಅಡ್ಡಸಾಲು, 0});
	}

	// Correction constants
	ಸ್ವಯಂಚಾಲಿತ ತಳ್ಳು_one_at = [&](ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು) {
		ಆದರೆ (ಲಂಬಸಾಲು < 0 || ಲಂಬಸಾಲು >= ಅಗಲ)
			ಹಿಂತಿರುಗಿ;
		std::ಸದಿಶ<RTLIL::State> v(ಅಗಲ, RTLIL::State::S0);
		v[ಲಂಬಸಾಲು] = RTLIL::State::S1;
		products.ತಳ್ಳು_ಹಿಂಭಾಗ({SigSpec(RTLIL::Const(v)), 0});
	};

	ಆದರೆ (b_signed)
		ತಳ್ಳು_one_at(ಅಗಲ_a - 1);
	ಆದರೆ (a_signed)
		ತಳ್ಳು_one_at(ಅಗಲ_b - 1);
	ಆದರೆ (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		ಪೂರ್ಣಾಂಕ sign_extension_ಆರಂಭ = ಅಗಲ_a + ಅಗಲ_b - 1;
		ಆದರೆ (sign_extension_ಆರಂಭ < ಅಗಲ) {
			std::ಸದಿಶ<RTLIL::State> v(ಅಗಲ, RTLIL::State::S0);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = sign_extension_ಆರಂಭ; i < ಅಗಲ; i++)
				v[i] = RTLIL::State::S1;
			products.ತಳ್ಳು_ಹಿಂಭಾಗ({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	ಹಿಂತಿರುಗಿ products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::ಸದಿಶ<ಆಳ_Sig> operands, ಪೂರ್ಣಾಂಕ ಅಗಲ, Strategy strategy, ಪೂರ್ಣಾಂಕ *ಹೊರಹೋಗು_compressor_ಎಣಿಕೆ, ಪೂರ್ಣಾಂಕ *ಹೊರಹೋಗು_final_ಆಳ) {
	ಪೂರ್ಣಾಂಕ ಮಟ್ಟಗಳು = 0;
	ಪೂರ್ಣಾಂಕ fa_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಂಕ c42_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಂಕ final_ಆಳ = 0;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &op : operands)
		op.sig.extend_u0(ಅಗಲ);

	// Only compress operands ready at current level
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ level = 0; operands.ಗಾತ್ರ_2() > 2; level++) {
		// Partition operands into ready and waiting
		std::ಸದಿಶ<ಆಳ_Sig> ready;
		std::ಸದಿಶ<ಆಳ_Sig> ಕಾಯುವಿಕೆ;
		ready.reserve(operands.ಗಾತ್ರ_2());
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &op : operands) {
			ಆದರೆ (op.ಆಳ <= level)
				ready.ತಳ್ಳು_ಹಿಂಭಾಗ(op);
			ಇಲ್ಲದಿದ್ದರೆ
				ಕಾಯುವಿಕೆ.ತಳ್ಳು_ಹಿಂಭಾಗ(op);
		}

		ಆದರೆ (ready.ಗಾತ್ರ_2() < 3) {
			ಮಟ್ಟಗಳು++;
			ಮುಂದುವರಿ;
		}

		// Apply compressors to ready operands
		std::ಸದಿಶ<ಆಳ_Sig> compressed;
		compressed.reserve(ready.ಗಾತ್ರ_2());
		ಗಾತ್ರ_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		ತಾರ್ಕಿಕ try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		ಆಗ (i < ready.ಗಾತ್ರ_2()) {
			ಗಾತ್ರ_t ಉಳಿದ = ready.ಗಾತ್ರ_2() - i;

			ಆದರೆ (try_42 && ಉಳಿದ >= 4) {
				ಆಳ_Sig a = ready[i + 0];
				ಆಳ_Sig b = ready[i + 1];
				ಆಳ_Sig c = ready[i + 2];
				ಆಳ_Sig d = ready[i + 3];

				ಸ್ವಯಂಚಾಲಿತ [ಮೊತ್ತ, ವಾಹಕ] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, ಅಗಲ);
				ಪೂರ್ಣಾಂಕ dmax = std::max({a.ಆಳ, b.ಆಳ, c.ಆಳ, d.ಆಳ});

				compressed.ತಳ್ಳು_ಹಿಂಭಾಗ({ಮೊತ್ತ, dmax + 2});
				compressed.ತಳ್ಳು_ಹಿಂಭಾಗ({ವಾಹಕ, dmax + 2});

				fa_ಎಣಿಕೆ += 2;
				c42_ಎಣಿಕೆ += 1;
				i += 4;
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಉಳಿದ >= 3) {
				ಆಳ_Sig a = ready[i + 0];
				ಆಳ_Sig b = ready[i + 1];
				ಆಳ_Sig c = ready[i + 2];

				ಸ್ವಯಂಚಾಲಿತ [ಮೊತ್ತ, ವಾಹಕ] = emit_compressor_32(module, a.sig, b.sig, c.sig, ಅಗಲ);
				ಪೂರ್ಣಾಂಕ dmax = std::max({a.ಆಳ, b.ಆಳ, c.ಆಳ});

				compressed.ತಳ್ಳು_ಹಿಂಭಾಗ({ಮೊತ್ತ, dmax + 1});
				compressed.ತಳ್ಳು_ಹಿಂಭಾಗ({ವಾಹಕ, dmax + 1});

				fa_ಎಣಿಕೆ += 1;
				i += 3;
			} ಇಲ್ಲದಿದ್ದರೆ {
				// Uncompressed operands pass through to next level
				ಗಾಗಿ (; i < ready.ಗಾತ್ರ_2(); i++)
					compressed.ತಳ್ಳು_ಹಿಂಭಾಗ(ready[i]);
				ನಿಲ್ಲಿಸು;
			}
		}

		// Merge compressed with waiting operands
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &op : ಕಾಯುವಿಕೆ)
			compressed.ತಳ್ಳು_ಹಿಂಭಾಗ(op);

		operands = std::move(compressed);
		ಮಟ್ಟಗಳು++;
	}

	ಆದರೆ(ಹೊರಹೋಗು_compressor_ಎಣಿಕೆ)
		*ಹೊರಹೋಗು_compressor_ಎಣಿಕೆ = fa_ಎಣಿಕೆ;
	ಆದರೆ (operands.ಗಾತ್ರ_2() == 0) {
		ಆದರೆ (ಹೊರಹೋಗು_final_ಆಳ)
			*ಹೊರಹೋಗು_final_ಆಳ = 0;
		ಹಿಂತಿರುಗಿ {SigSpec(State::S0, ಅಗಲ), SigSpec(State::S0, ಅಗಲ)};
	}
	ಆದರೆ (operands.ಗಾತ್ರ_2() == 1) {
		ಆದರೆ (ಹೊರಹೋಗು_final_ಆಳ)
			*ಹೊರಹೋಗು_final_ಆಳ = operands[0].ಆಳ;
		ಹಿಂತಿರುಗಿ {operands[0].sig, SigSpec(State::S0, ಅಗಲ)};
	}

	final_ಆಳ = std::max(operands[0].ಆಳ, operands[1].ಆಳ);
	ಆದರೆ (ಹೊರಹೋಗು_final_ಆಳ)
		*ಹೊರಹೋಗು_final_ಆಳ = final_ಆಳ;
	log_assert(operands.ಗಾತ್ರ_2() == 2);
	ಲಘುಗಣಕ("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", ಮಟ್ಟಗಳು, fa_ಎಣಿಕೆ, c42_ಎಣಿಕೆ, final_ಆಳ);
	ಹಿಂತಿರುಗಿ {operands[0].sig, operands[1].sig};
}

ಶೂನ್ಯ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(y);
	log_assert(Get_ಗಾತ್ರ(a) == ಅಗಲ);
	log_assert(Get_ಗಾತ್ರ(b) == ಅಗಲ);

	ಆದರೆ (ಅಗಲ == 0)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (ಅಗಲ == 1) {
		module->ಕೂಡಿಸು_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		ಹಿಂತಿರುಗಿ;
	}

	// Bit level gen and prop
	std::ಸದಿಶ<Sig_ಬಿಟ್> g_pre(ಅಗಲ), p_pre(ಅಗಲ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++) {
		Sig_ಬಿಟ್ gi = module->ಕೂಡಿಸು_Wire(NEW_ID);
		Sig_ಬಿಟ್ pi = module->ಕೂಡಿಸು_Wire(NEW_ID);
		module->ಕೂಡಿಸು_And_Gate(NEW_ID, a[i], b[i], gi);
		module->ಕೂಡಿಸು_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::ಸದಿಶ<Sig_ಬಿಟ್> g = g_pre;
	std::ಸದಿಶ<Sig_ಬಿಟ್> p = p_pre;
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಮಟ್ಟಗಳು = 0;

	ಆಗ ((1 << ಅಂಕೆ_ಮಟ್ಟಗಳು) < ಅಗಲ)
		ಅಂಕೆ_ಮಟ್ಟಗಳು++;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 1; k <= ಅಂಕೆ_ಮಟ್ಟಗಳು; k++) {
		ಪೂರ್ಣಾಂಕ s = 1 << (k - 1);
		std::ಸದಿಶ<Sig_ಬಿಟ್> g_ಮುಂದಿನ(ಅಗಲ), p_ಮುಂದಿನ(ಅಗಲ);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++) {
			ಆದರೆ (i < s) {
				// Nothing to do
				g_ಮುಂದಿನ[i] = g[i];
				p_ಮುಂದಿನ[i] = p[i];
			} ಇಲ್ಲದಿದ್ದರೆ {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_ಬಿಟ್ and_pg = module->ಕೂಡಿಸು_Wire(NEW_ID);
				module->ಕೂಡಿಸು_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_ಬಿಟ್ gnew = module->ಕೂಡಿಸು_Wire(NEW_ID);
				module->ಕೂಡಿಸು_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_ಮುಂದಿನ[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				ಆದರೆ (k < ಅಂಕೆ_ಮಟ್ಟಗಳು) {
					Sig_ಬಿಟ್ pnew = module->ಕೂಡಿಸು_Wire(NEW_ID);
					module->ಕೂಡಿಸು_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_ಮುಂದಿನ[i] = pnew;
				} ಇಲ್ಲದಿದ್ದರೆ {
					// Skip last level
					p_ಮುಂದಿನ[i] = State::Sx;
				}
			}
		}

		g = std::move(g_ಮುಂದಿನ);
		p = std::move(p_ಮುಂದಿನ);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಅಗಲ; i++)
		module->ಕೂಡಿಸು_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	ಆಯ್ಕೆ (choice) {
		ಸ್ಥಿತಿ FinalAdder::DEFAULT:
		ಸ್ಥಿತಿ FinalAdder::RIPPLE: {
			ಹಿಂತಿರುಗಿ module->ಕೂಡಿಸು_ಕೂಡಿಸು(NEW_ID, a, b, y, ಸುಳ್ಳು);
		}
		ಸ್ಥಿತಿ FinalAdder::PARALLEL_ಪೂರ್ವಪ್ರತ್ಯಯ: {
			emit_kogge_stone(module, a, b, y);
			ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
		}
	}
	log_assert(ಸುಳ್ಳು && "CompressorTree::emit_final_adder: invalid choice");
	ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

FinalAdder pick_final_adder(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ final_ಆಳ, Final_ಬಹುಲಕ ಬಹುಲಕ) {
	ಆಯ್ಕೆ (ಬಹುಲಕ) {
		ಸ್ಥಿತಿ Final_ಬಹುಲಕ::RIPPLE:  ಹಿಂತಿರುಗಿ FinalAdder::RIPPLE;
		ಸ್ಥಿತಿ Final_ಬಹುಲಕ::ಪೂರ್ವಪ್ರತ್ಯಯ:  ಹಿಂತಿರುಗಿ FinalAdder::PARALLEL_ಪೂರ್ವಪ್ರತ್ಯಯ;
		ಸ್ಥಿತಿ Final_ಬಹುಲಕ::AUTO:
		ಪೂರ್ವನಿಯೋಜಿತ: {
			ತಾರ್ಕಿಕ wide = ಅಗಲ >= RIPPLE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಅಗಲ_ಮಿತಿ;
			ತಾರ್ಕಿಕ deep = final_ಆಳ >= RIPPLE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಆಳ_ಮಿತಿ;
			ಹಿಂತಿರುಗಿ (wide && deep) ? FinalAdder::PARALLEL_ಪೂರ್ವಪ್ರತ್ಯಯ : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_ಕೊನೆ
