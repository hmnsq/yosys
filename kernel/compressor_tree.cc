#ਅਨ੍ਤਰ੍ਭਾਵ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ Compressor_ਰੁੱਖ
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, ਪੂਰਨ ਚੌੜਾਈ)
{
	SigSpec ਜੋੜ = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	SigSpec ਬਾਹਰ = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	module->ਜੋੜੋ_Fa(NEW_ID, a, b, c, ਬਾਹਰ, ਜੋੜ);

	SigSpec ਹਾਸਲ;
	ਹਾਸਲ.append(State::S0);
	ਹਾਸਲ.append(ਬਾਹਰ.ਕੱਢੋ_2(0, ਚੌੜਾਈ - 1));
	ਵਾਪਸ {ਜੋੜ, ਹਾਸਲ};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, ਪੂਰਨ ਚੌੜਾਈ)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	SigSpec cout_h_ਭਰਿਆ = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	module->ਜੋੜੋ_Fa(NEW_ID, a, b, c, cout_h_ਭਰਿਆ, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec ਅੰਦਰ;
	ਅੰਦਰ.append(State::S0);
	ਜੇ (ਚੌੜਾਈ > 1)
		ਅੰਦਰ.append(cout_h_ਭਰਿਆ.ਕੱਢੋ_2(0, ਚੌੜਾਈ - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec ਜੋੜ = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	SigSpec ਹਾਸਲ_ਭਰਿਆ = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	module->ਜੋੜੋ_Fa(NEW_ID, s0, d, ਅੰਦਰ, ਹਾਸਲ_ਭਰਿਆ, ਜੋੜ);

	SigSpec ਹਾਸਲ;
	ਹਾਸਲ.append(State::S0);
	ਜੇ (ਚੌੜਾਈ > 1)
		ਹਾਸਲ.append(ਹਾਸਲ_ਭਰਿਆ.ਕੱਢੋ_2(0, ਚੌੜਾਈ - 1));

	ਵਾਪਸ {ਜੋੜ, ਹਾਸਲ};
}

std::ਸਦਿਸ਼<ਡੂੰਘਾਈ_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, ਤਾਰਕਿਕ a_signed, ਤਾਰਕਿਕ b_signed, ਪੂਰਨ ਚੌੜਾਈ) {
	ਪੂਰਨ ਚੌੜਾਈ_a = Get_ਮਾਪ(a);
	ਪੂਰਨ ਚੌੜਾਈ_b = Get_ਮਾਪ(b);
	std::ਸਦਿਸ਼<ਡੂੰਘਾਈ_Sig> products;
	products.reserve(ਚੌੜਾਈ_a + 3);

	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ_a; i++) {
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(ਚੌੜਾਈ, ਝੂਠ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, ਚੌੜਾਈ);
		SigSpec ਪੰਕਤੀ = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
		module->ਜੋੜੋ_And(NEW_ID, b_shifted, ai_rep, ਪੰਕਤੀ);

		// Apply Modified Baugh-Wooley inversions for this row
		ਤਾਰਕਿਕ ਪੰਕਤੀ_is_bottom = (i == ਚੌੜਾਈ_a - 1);
		ਤਾਰਕਿਕ any_inversion = (ਪੰਕਤੀ_is_bottom && b_signed) || a_signed;

		ਜੇ (any_inversion) {
			std::ਸਦਿਸ਼<RTLIL::State> ਨਕਾਬ(ਚੌੜਾਈ, RTLIL::State::S0);

			ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ_b; j++) {
				ਪੂਰਨ ਖਾਨਾ = i + j;
				ਜੇ (ਖਾਨਾ < 0 || ਖਾਨਾ >= ਚੌੜਾਈ)
					ਜਾਰੀ;
				ਤਾਰਕਿਕ ਖਾਨਾ_is_ਸੱਜਾ = (j == ਚੌੜਾਈ_b - 1);
				// Flip masks
				ਤਾਰਕਿਕ invert = (ਪੰਕਤੀ_is_bottom && b_signed) ^ (ਖਾਨਾ_is_ਸੱਜਾ && a_signed);
				ਜੇ (invert)
					ਨਕਾਬ[ਖਾਨਾ] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			ਤਾਰਕਿਕ nonzero = ਝੂਠ;
			ਲਈ (ਸਵੈਚਾਲਿਤ s : ਨਕਾਬ)
				ਜੇ (s == RTLIL::State::S1) {
					nonzero = ਸੱਚ;
					ਰੋਕੋ;
				}
			ਜੇ (nonzero) {
				SigSpec inverted = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
				module->ਜੋੜੋ_Xor(NEW_ID, ਪੰਕਤੀ, SigSpec(RTLIL::Const(ਨਕਾਬ)), inverted);
				ਪੰਕਤੀ = inverted;
			}
		}

		products.ਧੱਕੋ_ਪਿੱਠ({ਪੰਕਤੀ, 0});
	}

	// Correction constants
	ਸਵੈਚਾਲਿਤ ਧੱਕੋ_one_at = [&](ਪੂਰਨ ਖਾਨਾ) {
		ਜੇ (ਖਾਨਾ < 0 || ਖਾਨਾ >= ਚੌੜਾਈ)
			ਵਾਪਸ;
		std::ਸਦਿਸ਼<RTLIL::State> v(ਚੌੜਾਈ, RTLIL::State::S0);
		v[ਖਾਨਾ] = RTLIL::State::S1;
		products.ਧੱਕੋ_ਪਿੱਠ({SigSpec(RTLIL::Const(v)), 0});
	};

	ਜੇ (b_signed)
		ਧੱਕੋ_one_at(ਚੌੜਾਈ_a - 1);
	ਜੇ (a_signed)
		ਧੱਕੋ_one_at(ਚੌੜਾਈ_b - 1);
	ਜੇ (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		ਪੂਰਨ sign_extension_ਸ਼ੁਰੂ = ਚੌੜਾਈ_a + ਚੌੜਾਈ_b - 1;
		ਜੇ (sign_extension_ਸ਼ੁਰੂ < ਚੌੜਾਈ) {
			std::ਸਦਿਸ਼<RTLIL::State> v(ਚੌੜਾਈ, RTLIL::State::S0);
			ਲਈ (ਪੂਰਨ i = sign_extension_ਸ਼ੁਰੂ; i < ਚੌੜਾਈ; i++)
				v[i] = RTLIL::State::S1;
			products.ਧੱਕੋ_ਪਿੱਠ({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	ਵਾਪਸ products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::ਸਦਿਸ਼<ਡੂੰਘਾਈ_Sig> operands, ਪੂਰਨ ਚੌੜਾਈ, Strategy strategy, ਪੂਰਨ *ਬਾਹਰਲਾ_compressor_ਗਿਣਤੀ, ਪੂਰਨ *ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ) {
	ਪੂਰਨ ਪੱਧਰ = 0;
	ਪੂਰਨ fa_ਗਿਣਤੀ = 0;
	ਪੂਰਨ c42_ਗਿਣਤੀ = 0;
	ਪੂਰਨ final_ਡੂੰਘਾਈ = 0;

	ਲਈ (ਸਵੈਚਾਲਿਤ &op : operands)
		op.sig.extend_u0(ਚੌੜਾਈ);

	// Only compress operands ready at current level
	ਲਈ (ਪੂਰਨ level = 0; operands.ਮਾਪ() > 2; level++) {
		// Partition operands into ready and waiting
		std::ਸਦਿਸ਼<ਡੂੰਘਾਈ_Sig> ready;
		std::ਸਦਿਸ਼<ਡੂੰਘਾਈ_Sig> ਉਡੀਕ;
		ready.reserve(operands.ਮਾਪ());
		ਲਈ (ਸਵੈਚਾਲਿਤ &op : operands) {
			ਜੇ (op.ਡੂੰਘਾਈ <= level)
				ready.ਧੱਕੋ_ਪਿੱਠ(op);
			ਨਹੀਂਤਾਂ
				ਉਡੀਕ.ਧੱਕੋ_ਪਿੱਠ(op);
		}

		ਜੇ (ready.ਮਾਪ() < 3) {
			ਪੱਧਰ++;
			ਜਾਰੀ;
		}

		// Apply compressors to ready operands
		std::ਸਦਿਸ਼<ਡੂੰਘਾਈ_Sig> compressed;
		compressed.reserve(ready.ਮਾਪ());
		ਮਾਪ_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		ਤਾਰਕਿਕ try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		ਜਦੋਂ (i < ready.ਮਾਪ()) {
			ਮਾਪ_t ਬਚਿਆ = ready.ਮਾਪ() - i;

			ਜੇ (try_42 && ਬਚਿਆ >= 4) {
				ਡੂੰਘਾਈ_Sig a = ready[i + 0];
				ਡੂੰਘਾਈ_Sig b = ready[i + 1];
				ਡੂੰਘਾਈ_Sig c = ready[i + 2];
				ਡੂੰਘਾਈ_Sig d = ready[i + 3];

				ਸਵੈਚਾਲਿਤ [ਜੋੜ, ਹਾਸਲ] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, ਚੌੜਾਈ);
				ਪੂਰਨ dmax = std::max({a.ਡੂੰਘਾਈ, b.ਡੂੰਘਾਈ, c.ਡੂੰਘਾਈ, d.ਡੂੰਘਾਈ});

				compressed.ਧੱਕੋ_ਪਿੱਠ({ਜੋੜ, dmax + 2});
				compressed.ਧੱਕੋ_ਪਿੱਠ({ਹਾਸਲ, dmax + 2});

				fa_ਗਿਣਤੀ += 2;
				c42_ਗਿਣਤੀ += 1;
				i += 4;
			} ਨਹੀਂਤਾਂ ਜੇ (ਬਚਿਆ >= 3) {
				ਡੂੰਘਾਈ_Sig a = ready[i + 0];
				ਡੂੰਘਾਈ_Sig b = ready[i + 1];
				ਡੂੰਘਾਈ_Sig c = ready[i + 2];

				ਸਵੈਚਾਲਿਤ [ਜੋੜ, ਹਾਸਲ] = emit_compressor_32(module, a.sig, b.sig, c.sig, ਚੌੜਾਈ);
				ਪੂਰਨ dmax = std::max({a.ਡੂੰਘਾਈ, b.ਡੂੰਘਾਈ, c.ਡੂੰਘਾਈ});

				compressed.ਧੱਕੋ_ਪਿੱਠ({ਜੋੜ, dmax + 1});
				compressed.ਧੱਕੋ_ਪਿੱਠ({ਹਾਸਲ, dmax + 1});

				fa_ਗਿਣਤੀ += 1;
				i += 3;
			} ਨਹੀਂਤਾਂ {
				// Uncompressed operands pass through to next level
				ਲਈ (; i < ready.ਮਾਪ(); i++)
					compressed.ਧੱਕੋ_ਪਿੱਠ(ready[i]);
				ਰੋਕੋ;
			}
		}

		// Merge compressed with waiting operands
		ਲਈ (ਸਵੈਚਾਲਿਤ &op : ਉਡੀਕ)
			compressed.ਧੱਕੋ_ਪਿੱਠ(op);

		operands = std::move(compressed);
		ਪੱਧਰ++;
	}

	ਜੇ(ਬਾਹਰਲਾ_compressor_ਗਿਣਤੀ)
		*ਬਾਹਰਲਾ_compressor_ਗਿਣਤੀ = fa_ਗਿਣਤੀ;
	ਜੇ (operands.ਮਾਪ() == 0) {
		ਜੇ (ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ)
			*ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ = 0;
		ਵਾਪਸ {SigSpec(State::S0, ਚੌੜਾਈ), SigSpec(State::S0, ਚੌੜਾਈ)};
	}
	ਜੇ (operands.ਮਾਪ() == 1) {
		ਜੇ (ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ)
			*ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ = operands[0].ਡੂੰਘਾਈ;
		ਵਾਪਸ {operands[0].sig, SigSpec(State::S0, ਚੌੜਾਈ)};
	}

	final_ਡੂੰਘਾਈ = std::max(operands[0].ਡੂੰਘਾਈ, operands[1].ਡੂੰਘਾਈ);
	ਜੇ (ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ)
		*ਬਾਹਰਲਾ_final_ਡੂੰਘਾਈ = final_ਡੂੰਘਾਈ;
	log_assert(operands.ਮਾਪ() == 2);
	ਲਘੂਗਣਕ("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", ਪੱਧਰ, fa_ਗਿਣਤੀ, c42_ਗਿਣਤੀ, final_ਡੂੰਘਾਈ);
	ਵਾਪਸ {operands[0].sig, operands[1].sig};
}

ਖਾਲੀ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(y);
	log_assert(Get_ਮਾਪ(a) == ਚੌੜਾਈ);
	log_assert(Get_ਮਾਪ(b) == ਚੌੜਾਈ);

	ਜੇ (ਚੌੜਾਈ == 0)
		ਵਾਪਸ;

	ਜੇ (ਚੌੜਾਈ == 1) {
		module->ਜੋੜੋ_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		ਵਾਪਸ;
	}

	// Bit level gen and prop
	std::ਸਦਿਸ਼<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> g_pre(ਚੌੜਾਈ), p_pre(ਚੌੜਾਈ);
	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++) {
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ gi = module->ਜੋੜੋ_Wire(NEW_ID);
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ pi = module->ਜੋੜੋ_Wire(NEW_ID);
		module->ਜੋੜੋ_And_Gate(NEW_ID, a[i], b[i], gi);
		module->ਜੋੜੋ_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::ਸਦਿਸ਼<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> g = g_pre;
	std::ਸਦਿਸ਼<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> p = p_pre;
	ਪੂਰਨ ਅੰਕ_ਪੱਧਰ = 0;

	ਜਦੋਂ ((1 << ਅੰਕ_ਪੱਧਰ) < ਚੌੜਾਈ)
		ਅੰਕ_ਪੱਧਰ++;

	ਲਈ (ਪੂਰਨ k = 1; k <= ਅੰਕ_ਪੱਧਰ; k++) {
		ਪੂਰਨ s = 1 << (k - 1);
		std::ਸਦਿਸ਼<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> g_ਅਗਲਾ(ਚੌੜਾਈ), p_ਅਗਲਾ(ਚੌੜਾਈ);
		ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++) {
			ਜੇ (i < s) {
				// Nothing to do
				g_ਅਗਲਾ[i] = g[i];
				p_ਅਗਲਾ[i] = p[i];
			} ਨਹੀਂਤਾਂ {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ and_pg = module->ਜੋੜੋ_Wire(NEW_ID);
				module->ਜੋੜੋ_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ gnew = module->ਜੋੜੋ_Wire(NEW_ID);
				module->ਜੋੜੋ_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_ਅਗਲਾ[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				ਜੇ (k < ਅੰਕ_ਪੱਧਰ) {
					Sig_ਦੋਆਧਾਰੀ_ਅੰਕ pnew = module->ਜੋੜੋ_Wire(NEW_ID);
					module->ਜੋੜੋ_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_ਅਗਲਾ[i] = pnew;
				} ਨਹੀਂਤਾਂ {
					// Skip last level
					p_ਅਗਲਾ[i] = State::Sx;
				}
			}
		}

		g = std::move(g_ਅਗਲਾ);
		p = std::move(p_ਅਗਲਾ);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	ਲਈ (ਪੂਰਨ i = 1; i < ਚੌੜਾਈ; i++)
		module->ਜੋੜੋ_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	ਚੋਣ (choice) {
		ਹਾਲਤ FinalAdder::DEFAULT:
		ਹਾਲਤ FinalAdder::RIPPLE: {
			ਵਾਪਸ module->ਜੋੜੋ_ਜੋੜੋ(NEW_ID, a, b, y, ਝੂਠ);
		}
		ਹਾਲਤ FinalAdder::PARALLEL_ਅਗੇਤਰ: {
			emit_kogge_stone(module, a, b, y);
			ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
		}
	}
	log_assert(ਝੂਠ && "CompressorTree::emit_final_adder: invalid choice");
	ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

FinalAdder pick_final_adder(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ final_ਡੂੰਘਾਈ, Final_ਬਹੁਲਕ ਬਹੁਲਕ) {
	ਚੋਣ (ਬਹੁਲਕ) {
		ਹਾਲਤ Final_ਬਹੁਲਕ::RIPPLE:  ਵਾਪਸ FinalAdder::RIPPLE;
		ਹਾਲਤ Final_ਬਹੁਲਕ::ਅਗੇਤਰ:  ਵਾਪਸ FinalAdder::PARALLEL_ਅਗੇਤਰ;
		ਹਾਲਤ Final_ਬਹੁਲਕ::AUTO:
		ਮੂਲ: {
			ਤਾਰਕਿਕ wide = ਚੌੜਾਈ >= RIPPLE_ਅਗੇਤਰ_ਚੌੜਾਈ_ਹੱਦ;
			ਤਾਰਕਿਕ deep = final_ਡੂੰਘਾਈ >= RIPPLE_ਅਗੇਤਰ_ਡੂੰਘਾਈ_ਹੱਦ;
			ਵਾਪਸ (wide && deep) ? FinalAdder::PARALLEL_ਅਗੇਤਰ : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_ਸਿਰਾ
