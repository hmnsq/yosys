#شامل "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

نامخلا Compressor_درخت
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحیح چوڑائی)
{
	SigSpec مجموع = module->جوڑو_Wire(NEW_ID, چوڑائی);
	SigSpec خارج = module->جوڑو_Wire(NEW_ID, چوڑائی);
	module->جوڑو_Fa(NEW_ID, a, b, c, خارج, مجموع);

	SigSpec حاصل;
	حاصل.append(State::S0);
	حاصل.append(خارج.نکالو_2(0, چوڑائی - 1));
	واپس {مجموع, حاصل};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحیح چوڑائی)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->جوڑو_Wire(NEW_ID, چوڑائی);
	SigSpec cout_h_بھرا = module->جوڑو_Wire(NEW_ID, چوڑائی);
	module->جوڑو_Fa(NEW_ID, a, b, c, cout_h_بھرا, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec داخل;
	داخل.append(State::S0);
	اگر (چوڑائی > 1)
		داخل.append(cout_h_بھرا.نکالو_2(0, چوڑائی - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec مجموع = module->جوڑو_Wire(NEW_ID, چوڑائی);
	SigSpec حاصل_بھرا = module->جوڑو_Wire(NEW_ID, چوڑائی);
	module->جوڑو_Fa(NEW_ID, s0, d, داخل, حاصل_بھرا, مجموع);

	SigSpec حاصل;
	حاصل.append(State::S0);
	اگر (چوڑائی > 1)
		حاصل.append(حاصل_بھرا.نکالو_2(0, چوڑائی - 1));

	واپس {مجموع, حاصل};
}

std::سمتیہ<گہرائی_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, منطقی a_signed, منطقی b_signed, صحیح چوڑائی) {
	صحیح چوڑائی_a = Get_سائز(a);
	صحیح چوڑائی_b = Get_سائز(b);
	std::سمتیہ<گہرائی_Sig> products;
	products.reserve(چوڑائی_a + 3);

	برائے (صحیح i = 0; i < چوڑائی_a; i++) {
		Sig_ثنائی_ہندسہ ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(چوڑائی, جھوٹ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, چوڑائی);
		SigSpec صف = module->جوڑو_Wire(NEW_ID, چوڑائی);
		module->جوڑو_And(NEW_ID, b_shifted, ai_rep, صف);

		// Apply Modified Baugh-Wooley inversions for this row
		منطقی صف_is_bottom = (i == چوڑائی_a - 1);
		منطقی any_inversion = (صف_is_bottom && b_signed) || a_signed;

		اگر (any_inversion) {
			std::سمتیہ<RTLIL::State> نقاب(چوڑائی, RTLIL::State::S0);

			برائے (صحیح j = 0; j < چوڑائی_b; j++) {
				صحیح ستون = i + j;
				اگر (ستون < 0 || ستون >= چوڑائی)
					جاری;
				منطقی ستون_is_دایاں = (j == چوڑائی_b - 1);
				// Flip masks
				منطقی invert = (صف_is_bottom && b_signed) ^ (ستون_is_دایاں && a_signed);
				اگر (invert)
					نقاب[ستون] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			منطقی nonzero = جھوٹ;
			برائے (خودکار s : نقاب)
				اگر (s == RTLIL::State::S1) {
					nonzero = سچ;
					توڑو;
				}
			اگر (nonzero) {
				SigSpec inverted = module->جوڑو_Wire(NEW_ID, چوڑائی);
				module->جوڑو_Xor(NEW_ID, صف, SigSpec(RTLIL::Const(نقاب)), inverted);
				صف = inverted;
			}
		}

		products.ڈالو_عقب({صف, 0});
	}

	// Correction constants
	خودکار ڈالو_one_at = [&](صحیح ستون) {
		اگر (ستون < 0 || ستون >= چوڑائی)
			واپس;
		std::سمتیہ<RTLIL::State> v(چوڑائی, RTLIL::State::S0);
		v[ستون] = RTLIL::State::S1;
		products.ڈالو_عقب({SigSpec(RTLIL::Const(v)), 0});
	};

	اگر (b_signed)
		ڈالو_one_at(چوڑائی_a - 1);
	اگر (a_signed)
		ڈالو_one_at(چوڑائی_b - 1);
	اگر (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		صحیح sign_extension_آغاز = چوڑائی_a + چوڑائی_b - 1;
		اگر (sign_extension_آغاز < چوڑائی) {
			std::سمتیہ<RTLIL::State> v(چوڑائی, RTLIL::State::S0);
			برائے (صحیح i = sign_extension_آغاز; i < چوڑائی; i++)
				v[i] = RTLIL::State::S1;
			products.ڈالو_عقب({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	واپس products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::سمتیہ<گہرائی_Sig> operands, صحیح چوڑائی, Strategy strategy, صحیح *برآمد_compressor_گنتی, صحیح *برآمد_final_گہرائی) {
	صحیح سطحیں = 0;
	صحیح fa_گنتی = 0;
	صحیح c42_گنتی = 0;
	صحیح final_گہرائی = 0;

	برائے (خودکار &op : operands)
		op.sig.extend_u0(چوڑائی);

	// Only compress operands ready at current level
	برائے (صحیح level = 0; operands.سائز() > 2; level++) {
		// Partition operands into ready and waiting
		std::سمتیہ<گہرائی_Sig> ready;
		std::سمتیہ<گہرائی_Sig> انتظار;
		ready.reserve(operands.سائز());
		برائے (خودکار &op : operands) {
			اگر (op.گہرائی <= level)
				ready.ڈالو_عقب(op);
			ورنہ
				انتظار.ڈالو_عقب(op);
		}

		اگر (ready.سائز() < 3) {
			سطحیں++;
			جاری;
		}

		// Apply compressors to ready operands
		std::سمتیہ<گہرائی_Sig> compressed;
		compressed.reserve(ready.سائز());
		سائز_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		منطقی try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		جب (i < ready.سائز()) {
			سائز_t بقایا = ready.سائز() - i;

			اگر (try_42 && بقایا >= 4) {
				گہرائی_Sig a = ready[i + 0];
				گہرائی_Sig b = ready[i + 1];
				گہرائی_Sig c = ready[i + 2];
				گہرائی_Sig d = ready[i + 3];

				خودکار [مجموع, حاصل] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, چوڑائی);
				صحیح dmax = std::max({a.گہرائی, b.گہرائی, c.گہرائی, d.گہرائی});

				compressed.ڈالو_عقب({مجموع, dmax + 2});
				compressed.ڈالو_عقب({حاصل, dmax + 2});

				fa_گنتی += 2;
				c42_گنتی += 1;
				i += 4;
			} ورنہ اگر (بقایا >= 3) {
				گہرائی_Sig a = ready[i + 0];
				گہرائی_Sig b = ready[i + 1];
				گہرائی_Sig c = ready[i + 2];

				خودکار [مجموع, حاصل] = emit_compressor_32(module, a.sig, b.sig, c.sig, چوڑائی);
				صحیح dmax = std::max({a.گہرائی, b.گہرائی, c.گہرائی});

				compressed.ڈالو_عقب({مجموع, dmax + 1});
				compressed.ڈالو_عقب({حاصل, dmax + 1});

				fa_گنتی += 1;
				i += 3;
			} ورنہ {
				// Uncompressed operands pass through to next level
				برائے (; i < ready.سائز(); i++)
					compressed.ڈالو_عقب(ready[i]);
				توڑو;
			}
		}

		// Merge compressed with waiting operands
		برائے (خودکار &op : انتظار)
			compressed.ڈالو_عقب(op);

		operands = std::move(compressed);
		سطحیں++;
	}

	اگر(برآمد_compressor_گنتی)
		*برآمد_compressor_گنتی = fa_گنتی;
	اگر (operands.سائز() == 0) {
		اگر (برآمد_final_گہرائی)
			*برآمد_final_گہرائی = 0;
		واپس {SigSpec(State::S0, چوڑائی), SigSpec(State::S0, چوڑائی)};
	}
	اگر (operands.سائز() == 1) {
		اگر (برآمد_final_گہرائی)
			*برآمد_final_گہرائی = operands[0].گہرائی;
		واپس {operands[0].sig, SigSpec(State::S0, چوڑائی)};
	}

	final_گہرائی = std::max(operands[0].گہرائی, operands[1].گہرائی);
	اگر (برآمد_final_گہرائی)
		*برآمد_final_گہرائی = final_گہرائی;
	log_assert(operands.سائز() == 2);
	لوگارتھم("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", سطحیں, fa_گنتی, c42_گنتی, final_گہرائی);
	واپس {operands[0].sig, operands[1].sig};
}

خالی emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	صحیح چوڑائی = Get_سائز(y);
	log_assert(Get_سائز(a) == چوڑائی);
	log_assert(Get_سائز(b) == چوڑائی);

	اگر (چوڑائی == 0)
		واپس;

	اگر (چوڑائی == 1) {
		module->جوڑو_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		واپس;
	}

	// Bit level gen and prop
	std::سمتیہ<Sig_ثنائی_ہندسہ> g_pre(چوڑائی), p_pre(چوڑائی);
	برائے (صحیح i = 0; i < چوڑائی; i++) {
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
	صحیح عدد_سطحیں = 0;

	جب ((1 << عدد_سطحیں) < چوڑائی)
		عدد_سطحیں++;

	برائے (صحیح k = 1; k <= عدد_سطحیں; k++) {
		صحیح s = 1 << (k - 1);
		std::سمتیہ<Sig_ثنائی_ہندسہ> g_اگلا(چوڑائی), p_اگلا(چوڑائی);
		برائے (صحیح i = 0; i < چوڑائی; i++) {
			اگر (i < s) {
				// Nothing to do
				g_اگلا[i] = g[i];
				p_اگلا[i] = p[i];
			} ورنہ {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_ثنائی_ہندسہ and_pg = module->جوڑو_Wire(NEW_ID);
				module->جوڑو_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_ثنائی_ہندسہ gnew = module->جوڑو_Wire(NEW_ID);
				module->جوڑو_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_اگلا[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				اگر (k < عدد_سطحیں) {
					Sig_ثنائی_ہندسہ pnew = module->جوڑو_Wire(NEW_ID);
					module->جوڑو_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_اگلا[i] = pnew;
				} ورنہ {
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
	برائے (صحیح i = 1; i < چوڑائی; i++)
		module->جوڑو_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	انتخاب (choice) {
		حالت FinalAdder::DEFAULT:
		حالت FinalAdder::RIPPLE: {
			واپس module->جوڑو_جوڑو(NEW_ID, a, b, y, جھوٹ);
		}
		حالت FinalAdder::PARALLEL_سابقہ: {
			emit_kogge_stone(module, a, b, y);
			واپس خالی_اشاریہ;
		}
	}
	log_assert(جھوٹ && "CompressorTree::emit_final_adder: invalid choice");
	واپس خالی_اشاریہ;
}

FinalAdder pick_final_adder(صحیح چوڑائی, صحیح final_گہرائی, Final_منوال منوال) {
	انتخاب (منوال) {
		حالت Final_منوال::RIPPLE:  واپس FinalAdder::RIPPLE;
		حالت Final_منوال::سابقہ:  واپس FinalAdder::PARALLEL_سابقہ;
		حالت Final_منوال::AUTO:
		طےشدہ: {
			منطقی wide = چوڑائی >= RIPPLE_سابقہ_چوڑائی_حد;
			منطقی deep = final_گہرائی >= RIPPLE_سابقہ_گہرائی_حد;
			واپس (wide && deep) ? FinalAdder::PARALLEL_سابقہ : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_انتہا
