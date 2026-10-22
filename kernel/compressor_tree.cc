#ضمن "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

نطاق Compressor_درخت
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحيح چوڑائی)
{
	SigSpec جوڑ = module->جوڑو_Wire(NEW_ID, چوڑائی);
	SigSpec باہر = module->جوڑو_Wire(NEW_ID, چوڑائی);
	module->جوڑو_Fa(NEW_ID, a, b, c, باہر, جوڑ);

	SigSpec حاصل;
	حاصل.append(State::S0);
	حاصل.append(باہر.کڈھو(0, چوڑائی - 1));
	أرجع {جوڑ, حاصل};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحيح چوڑائی)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->جوڑو_Wire(NEW_ID, چوڑائی);
	SigSpec cout_h_بھریا = module->جوڑو_Wire(NEW_ID, چوڑائی);
	module->جوڑو_Fa(NEW_ID, a, b, c, cout_h_بھریا, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec اندر;
	اندر.append(State::S0);
	إذا (چوڑائی > 1)
		اندر.append(cout_h_بھریا.کڈھو(0, چوڑائی - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec جوڑ = module->جوڑو_Wire(NEW_ID, چوڑائی);
	SigSpec حاصل_بھریا = module->جوڑو_Wire(NEW_ID, چوڑائی);
	module->جوڑو_Fa(NEW_ID, s0, d, اندر, حاصل_بھریا, جوڑ);

	SigSpec حاصل;
	حاصل.append(State::S0);
	إذا (چوڑائی > 1)
		حاصل.append(حاصل_بھریا.کڈھو(0, چوڑائی - 1));

	أرجع {جوڑ, حاصل};
}

std::سمتیہ<ڈونگھائی_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, منطقي a_signed, منطقي b_signed, صحيح چوڑائی) {
	صحيح چوڑائی_a = Get_ناپ(a);
	صحيح چوڑائی_b = Get_ناپ(b);
	std::سمتیہ<ڈونگھائی_Sig> products;
	products.reserve(چوڑائی_a + 3);

	لكل (صحيح i = 0; i < چوڑائی_a; i++) {
		Sig_ثنائی_ہندسہ ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(چوڑائی, خطأ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, چوڑائی);
		SigSpec صف = module->جوڑو_Wire(NEW_ID, چوڑائی);
		module->جوڑو_And(NEW_ID, b_shifted, ai_rep, صف);

		// Apply Modified Baugh-Wooley inversions for this row
		منطقي صف_is_bottom = (i == چوڑائی_a - 1);
		منطقي any_inversion = (صف_is_bottom && b_signed) || a_signed;

		إذا (any_inversion) {
			std::سمتیہ<RTLIL::State> نقاب(چوڑائی, RTLIL::State::S0);

			لكل (صحيح j = 0; j < چوڑائی_b; j++) {
				صحيح خانہ = i + j;
				إذا (خانہ < 0 || خانہ >= چوڑائی)
					استمر;
				منطقي خانہ_is_دایاں = (j == چوڑائی_b - 1);
				// Flip masks
				منطقي invert = (صف_is_bottom && b_signed) ^ (خانہ_is_دایاں && a_signed);
				إذا (invert)
					نقاب[خانہ] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			منطقي nonzero = خطأ;
			لكل (خودکار s : نقاب)
				إذا (s == RTLIL::State::S1) {
					nonzero = سچ;
					اكسر;
				}
			إذا (nonzero) {
				SigSpec inverted = module->جوڑو_Wire(NEW_ID, چوڑائی);
				module->جوڑو_Xor(NEW_ID, صف, SigSpec(RTLIL::Const(نقاب)), inverted);
				صف = inverted;
			}
		}

		products.ڈالو_پٹھ({صف, 0});
	}

	// Correction constants
	خودکار ڈالو_one_at = [&](صحيح خانہ) {
		إذا (خانہ < 0 || خانہ >= چوڑائی)
			أرجع;
		std::سمتیہ<RTLIL::State> v(چوڑائی, RTLIL::State::S0);
		v[خانہ] = RTLIL::State::S1;
		products.ڈالو_پٹھ({SigSpec(RTLIL::Const(v)), 0});
	};

	إذا (b_signed)
		ڈالو_one_at(چوڑائی_a - 1);
	إذا (a_signed)
		ڈالو_one_at(چوڑائی_b - 1);
	إذا (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		صحيح sign_extension_شروع = چوڑائی_a + چوڑائی_b - 1;
		إذا (sign_extension_شروع < چوڑائی) {
			std::سمتیہ<RTLIL::State> v(چوڑائی, RTLIL::State::S0);
			لكل (صحيح i = sign_extension_شروع; i < چوڑائی; i++)
				v[i] = RTLIL::State::S1;
			products.ڈالو_پٹھ({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	أرجع products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::سمتیہ<ڈونگھائی_Sig> operands, صحيح چوڑائی, Strategy strategy, صحيح *باہرلا_compressor_گنتی, صحيح *باہرلا_final_ڈونگھائی) {
	صحيح پدھراں = 0;
	صحيح fa_گنتی = 0;
	صحيح c42_گنتی = 0;
	صحيح final_ڈونگھائی = 0;

	لكل (خودکار &op : operands)
		op.sig.extend_u0(چوڑائی);

	// Only compress operands ready at current level
	لكل (صحيح level = 0; operands.ناپ() > 2; level++) {
		// Partition operands into ready and waiting
		std::سمتیہ<ڈونگھائی_Sig> ready;
		std::سمتیہ<ڈونگھائی_Sig> اڈیک;
		ready.reserve(operands.ناپ());
		لكل (خودکار &op : operands) {
			إذا (op.ڈونگھائی <= level)
				ready.ڈالو_پٹھ(op);
			وإلا
				اڈیک.ڈالو_پٹھ(op);
		}

		إذا (ready.ناپ() < 3) {
			پدھراں++;
			استمر;
		}

		// Apply compressors to ready operands
		std::سمتیہ<ڈونگھائی_Sig> compressed;
		compressed.reserve(ready.ناپ());
		ناپ_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		منطقي try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		طالما (i < ready.ناپ()) {
			ناپ_t بچیا = ready.ناپ() - i;

			إذا (try_42 && بچیا >= 4) {
				ڈونگھائی_Sig a = ready[i + 0];
				ڈونگھائی_Sig b = ready[i + 1];
				ڈونگھائی_Sig c = ready[i + 2];
				ڈونگھائی_Sig d = ready[i + 3];

				خودکار [جوڑ, حاصل] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, چوڑائی);
				صحيح dmax = std::max({a.ڈونگھائی, b.ڈونگھائی, c.ڈونگھائی, d.ڈونگھائی});

				compressed.ڈالو_پٹھ({جوڑ, dmax + 2});
				compressed.ڈالو_پٹھ({حاصل, dmax + 2});

				fa_گنتی += 2;
				c42_گنتی += 1;
				i += 4;
			} وإلا إذا (بچیا >= 3) {
				ڈونگھائی_Sig a = ready[i + 0];
				ڈونگھائی_Sig b = ready[i + 1];
				ڈونگھائی_Sig c = ready[i + 2];

				خودکار [جوڑ, حاصل] = emit_compressor_32(module, a.sig, b.sig, c.sig, چوڑائی);
				صحيح dmax = std::max({a.ڈونگھائی, b.ڈونگھائی, c.ڈونگھائی});

				compressed.ڈالو_پٹھ({جوڑ, dmax + 1});
				compressed.ڈالو_پٹھ({حاصل, dmax + 1});

				fa_گنتی += 1;
				i += 3;
			} وإلا {
				// Uncompressed operands pass through to next level
				لكل (; i < ready.ناپ(); i++)
					compressed.ڈالو_پٹھ(ready[i]);
				اكسر;
			}
		}

		// Merge compressed with waiting operands
		لكل (خودکار &op : اڈیک)
			compressed.ڈالو_پٹھ(op);

		operands = std::move(compressed);
		پدھراں++;
	}

	إذا(باہرلا_compressor_گنتی)
		*باہرلا_compressor_گنتی = fa_گنتی;
	إذا (operands.ناپ() == 0) {
		إذا (باہرلا_final_ڈونگھائی)
			*باہرلا_final_ڈونگھائی = 0;
		أرجع {SigSpec(State::S0, چوڑائی), SigSpec(State::S0, چوڑائی)};
	}
	إذا (operands.ناپ() == 1) {
		إذا (باہرلا_final_ڈونگھائی)
			*باہرلا_final_ڈونگھائی = operands[0].ڈونگھائی;
		أرجع {operands[0].sig, SigSpec(State::S0, چوڑائی)};
	}

	final_ڈونگھائی = std::max(operands[0].ڈونگھائی, operands[1].ڈونگھائی);
	إذا (باہرلا_final_ڈونگھائی)
		*باہرلا_final_ڈونگھائی = final_ڈونگھائی;
	log_assert(operands.ناپ() == 2);
	لوگارتھم("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", پدھراں, fa_گنتی, c42_گنتی, final_ڈونگھائی);
	أرجع {operands[0].sig, operands[1].sig};
}

فراغ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	صحيح چوڑائی = Get_ناپ(y);
	log_assert(Get_ناپ(a) == چوڑائی);
	log_assert(Get_ناپ(b) == چوڑائی);

	إذا (چوڑائی == 0)
		أرجع;

	إذا (چوڑائی == 1) {
		module->جوڑو_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		أرجع;
	}

	// Bit level gen and prop
	std::سمتیہ<Sig_ثنائی_ہندسہ> g_pre(چوڑائی), p_pre(چوڑائی);
	لكل (صحيح i = 0; i < چوڑائی; i++) {
		Sig_ثنائی_ہندسہ gi = module->جوڑو_Wire(NEW_ID);
		Sig_ثنائی_ہندسہ pi = module->جوڑو_Wire(NEW_ID);
		module->جوڑو_And_Gate(NEW_ID, a[i], b[i], gi);
		module->جوڑو_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::سمتیہ<Sig_ثنائی_ہندسہ> g = g_pre;
	std::سمتیہ<Sig_ثنائی_ہندسہ> p = p_pre;
	صحيح عدد_پدھراں = 0;

	طالما ((1 << عدد_پدھراں) < چوڑائی)
		عدد_پدھراں++;

	لكل (صحيح k = 1; k <= عدد_پدھراں; k++) {
		صحيح s = 1 << (k - 1);
		std::سمتیہ<Sig_ثنائی_ہندسہ> g_اگلا(چوڑائی), p_اگلا(چوڑائی);
		لكل (صحيح i = 0; i < چوڑائی; i++) {
			إذا (i < s) {
				// Nothing to do
				g_اگلا[i] = g[i];
				p_اگلا[i] = p[i];
			} وإلا {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_ثنائی_ہندسہ and_pg = module->جوڑو_Wire(NEW_ID);
				module->جوڑو_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_ثنائی_ہندسہ gnew = module->جوڑو_Wire(NEW_ID);
				module->جوڑو_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_اگلا[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				إذا (k < عدد_پدھراں) {
					Sig_ثنائی_ہندسہ pnew = module->جوڑو_Wire(NEW_ID);
					module->جوڑو_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_اگلا[i] = pnew;
				} وإلا {
					// Skip last level
					p_اگلا[i] = State::Sx;
				}
			}
		}

		g = std::move(g_اگلا);
		p = std::move(p_اگلا);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	لكل (صحيح i = 1; i < چوڑائی; i++)
		module->جوڑو_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	بدل (choice) {
		حالة FinalAdder::DEFAULT:
		حالة FinalAdder::RIPPLE: {
			أرجع module->جوڑو_جوڑو(NEW_ID, a, b, y, خطأ);
		}
		حالة FinalAdder::PARALLEL_اگیتر: {
			emit_kogge_stone(module, a, b, y);
			أرجع خالی_سوچک;
		}
	}
	log_assert(خطأ && "CompressorTree::emit_final_adder: invalid choice");
	أرجع خالی_سوچک;
}

FinalAdder pick_final_adder(صحيح چوڑائی, صحيح final_ڈونگھائی, Final_منوال منوال) {
	بدل (منوال) {
		حالة Final_منوال::RIPPLE:  أرجع FinalAdder::RIPPLE;
		حالة Final_منوال::اگیتر:  أرجع FinalAdder::PARALLEL_اگیتر;
		حالة Final_منوال::AUTO:
		افتراضي: {
			منطقي wide = چوڑائی >= RIPPLE_اگیتر_چوڑائی_حد;
			منطقي deep = final_ڈونگھائی >= RIPPLE_اگیتر_ڈونگھائی_حد;
			أرجع (wide && deep) ? FinalAdder::PARALLEL_اگیتر : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_سرا
