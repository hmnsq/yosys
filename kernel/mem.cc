/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Marcelina Kościelnicka <mwk@0x04.net>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/mem.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ff.h"

USING_YOSYS_NAMESPACE

ਖਾਲੀ Mem::remove() {
	ਜੇ (cell) {
		module->remove(cell);
		cell = ਖਾਲੀ_ਸੂਚਕ;
	}
	ਜੇ (mem) {
		module->memories.erase(mem->name);
		ਮਿਟਾਓ mem;
		mem = ਖਾਲੀ_ਸੂਚਕ;
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : rd_ports) {
		ਜੇ (ਪੋਰਟ.cell) {
			module->remove(ਪੋਰਟ.cell);
			ਪੋਰਟ.cell = ਖਾਲੀ_ਸੂਚਕ;
		}
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : wr_ports) {
		ਜੇ (ਪੋਰਟ.cell) {
			module->remove(ਪੋਰਟ.cell);
			ਪੋਰਟ.cell = ਖਾਲੀ_ਸੂਚਕ;
		}
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &init : inits) {
		ਜੇ (init.cell) {
			module->remove(init.cell);
			init.cell = ਖਾਲੀ_ਸੂਚਕ;
		}
	}
}

ਖਾਲੀ Mem::emit() {
	check();
	std::ਸਦਿਸ਼<ਪੂਰਨ> rd_ਖੱਬਾ;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ports); i++) {
		ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = rd_ports[i];
		ਜੇ (ਪੋਰਟ.removed) {
			ਜੇ (ਪੋਰਟ.cell) {
				module->remove(ਪੋਰਟ.cell);
			}
		} ਨਹੀਂਤਾਂ {
			rd_ਖੱਬਾ.ਧੱਕੋ_ਪਿੱਠ(i);
		}
	}
	std::ਸਦਿਸ਼<ਪੂਰਨ> wr_ਖੱਬਾ;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++) {
		ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = wr_ports[i];
		ਜੇ (ਪੋਰਟ.removed) {
			ਜੇ (ਪੋਰਟ.cell) {
				module->remove(ਪੋਰਟ.cell);
			}
		} ਨਹੀਂਤਾਂ {
			wr_ਖੱਬਾ.ਧੱਕੋ_ਪਿੱਠ(i);
		}
	}
	std::ਸਦਿਸ਼<ਪੂਰਨ> init_ਖੱਬਾ;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(inits); i++) {
		ਸਵੈਚਾਲਿਤ &init = inits[i];
		ਜੇ (init.removed) {
			ਜੇ (init.cell) {
				module->remove(init.cell);
			}
		} ਨਹੀਂਤਾਂ {
			init_ਖੱਬਾ.ਧੱਕੋ_ਪਿੱਠ(i);
		}
	}
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ਖੱਬਾ); i++)
		ਜੇ (i != rd_ਖੱਬਾ[i])
			std::ਅਦਲਾ_ਬਦਲੀ(rd_ports[i], rd_ports[rd_ਖੱਬਾ[i]]);
	rd_ports.resize(Get_ਮਾਪ(rd_ਖੱਬਾ));
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ਖੱਬਾ); i++)
		ਜੇ (i != wr_ਖੱਬਾ[i])
			std::ਅਦਲਾ_ਬਦਲੀ(wr_ports[i], wr_ports[wr_ਖੱਬਾ[i]]);
	wr_ports.resize(Get_ਮਾਪ(wr_ਖੱਬਾ));
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(init_ਖੱਬਾ); i++)
		ਜੇ (i != init_ਖੱਬਾ[i])
			std::ਅਦਲਾ_ਬਦਲੀ(inits[i], inits[init_ਖੱਬਾ[i]]);
	inits.resize(Get_ਮਾਪ(init_ਖੱਬਾ));

	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : rd_ports) {
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ਖੱਬਾ); i++) {
			ਪੋਰਟ.transparency_ਨਕਾਬ[i] = ਪੋਰਟ.transparency_ਨਕਾਬ[wr_ਖੱਬਾ[i]];
			ਪੋਰਟ.collision_x_ਨਕਾਬ[i] = ਪੋਰਟ.collision_x_ਨਕਾਬ[wr_ਖੱਬਾ[i]];
		}
		ਪੋਰਟ.transparency_ਨਕਾਬ.resize(Get_ਮਾਪ(wr_ਖੱਬਾ));
		ਪੋਰਟ.collision_x_ਨਕਾਬ.resize(Get_ਮਾਪ(wr_ਖੱਬਾ));
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : wr_ports) {
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ਖੱਬਾ); i++)
			ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ[i] = ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ[wr_ਖੱਬਾ[i]];
		ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ.resize(Get_ਮਾਪ(wr_ਖੱਬਾ));
	}

	ਜੇ (packed) {
		ਜੇ (mem) {
			module->memories.erase(mem->name);
			ਮਿਟਾਓ mem;
			mem = ਖਾਲੀ_ਸੂਚਕ;
		}
		ਜੇ (!cell) {
			ਜੇ (memid.ਖਾਲੀ_ਥਾਵਾਂ())
				memid = PooledName(module->design, module->twines().ਜੋੜੋ(NEW_ID));
			cell = module->ਜੋੜੋ_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->ਗੁਣ = ਗੁਣ;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::ਚੌੜਾਈ_2] = Const(ਚੌੜਾਈ);
		cell->parameters[ID::ਵਿੱਥ_2] = Const(ਸ਼ੁਰੂ_ਵਿੱਥ);
		cell->parameters[ID::ਮਾਪ_2] = Const(ਮਾਪ);
		SigSpec rd_clk, rd_en, rd_addr, rd_ਅੰਕੜੇ;
		SigSpec wr_clk, wr_en, wr_addr, wr_ਅੰਕੜੇ;
		SigSpec rd_arst, rd_srst;
		ਪੂਰਨ abits = 0;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : rd_ports)
			abits = std::max(abits, Get_ਮਾਪ(ਪੋਰਟ.addr));
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : wr_ports)
			abits = std::max(abits, Get_ਮਾਪ(ਪੋਰਟ.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::ਸਦਿਸ਼<ਪੂਰਨ> wr_ਪੋਰਟ_xlat;
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++)
			ਲਈ (ਪੂਰਨ j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_ਪੋਰਟ_xlat.ਧੱਕੋ_ਪਿੱਠ(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_ਨਕਾਬ_builder;
		Const::Builder rd_collision_x_ਨਕਾਬ_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_ਮੁੱਲ_builder;
		Const::Builder rd_srst_ਮੁੱਲ_builder;
		Const::Builder rd_init_ਮੁੱਲ_builder;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : rd_ports) {
			ਲਈ (ਸਵੈਚਾਲਿਤ attr: ਪੋਰਟ.ਗੁਣ)
				ਜੇ (!cell->has_attribute(attr.ਪਹਿਲਾ))
					cell->ਗੁਣ.ਪਾਓ(attr);
			ਜੇ (ਪੋਰਟ.cell) {
				module->remove(ਪੋਰਟ.cell);
				ਪੋਰਟ.cell = ਖਾਲੀ_ਸੂਚਕ;
			}
			ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << ਪੋਰਟ.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ਧੱਕੋ_ਪਿੱਠ(State(sub != 0));
				rd_clk_enable_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਪੋਰਟ.clk_enable));
				rd_clk_polarity_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਪੋਰਟ.clk_polarity));
				rd_ce_over_srst_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਪੋਰਟ.ce_over_srst));
				rd_clk.append(ਪੋਰਟ.clk);
				rd_arst.append(ਪੋਰਟ.arst);
				rd_srst.append(ਪੋਰਟ.srst);
				rd_en.append(ਪੋਰਟ.en);
				SigSpec addr = ਪੋਰਟ.sub_addr(sub);
				addr.extend_u0(abits, ਝੂਠ);
				rd_addr.append(addr);
				log_assert(Get_ਮਾਪ(addr) == abits);
				ਲਈ (ਸਵੈਚਾਲਿਤ idx : wr_ਪੋਰਟ_xlat) {
					rd_transparency_ਨਕਾਬ_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਤਾਰਕਿਕ(ਪੋਰਟ.transparency_ਨਕਾਬ[idx])));
					rd_collision_x_ਨਕਾਬ_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਤਾਰਕਿਕ(ਪੋਰਟ.collision_x_ਨਕਾਬ[idx])));
				}
			}
			rd_ਅੰਕੜੇ.append(ਪੋਰਟ.ਅੰਕੜੇ);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਪੋਰਟ.arst_ਮੁੱਲ)
				rd_arst_ਮੁੱਲ_builder.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਪੋਰਟ.srst_ਮੁੱਲ)
				rd_srst_ਮੁੱਲ_builder.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਪੋਰਟ.init_ਮੁੱਲ)
				rd_init_ਮੁੱਲ_builder.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_ਨਕਾਬ = rd_transparency_ਨਕਾਬ_builder.build();
		Const rd_collision_x_ਨਕਾਬ = rd_collision_x_ਨਕਾਬ_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_ਮੁੱਲ = rd_arst_ਮੁੱਲ_builder.build();
		Const rd_srst_ਮੁੱਲ = rd_srst_ਮੁੱਲ_builder.build();
		Const rd_init_ਮੁੱਲ = rd_init_ਮੁੱਲ_builder.build();
		ਜੇ (rd_ports.ਖਾਲੀ_ਥਾਵਾਂ()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_ਮੁੱਲ = State::S0;
			rd_srst_ਮੁੱਲ = State::S0;
			rd_init_ਮੁੱਲ = State::S0;
		}
		ਜੇ (rd_ports.ਖਾਲੀ_ਥਾਵਾਂ() || wr_ports.ਖਾਲੀ_ਥਾਵਾਂ()) {
			rd_transparency_ਨਕਾਬ = State::S0;
			rd_collision_x_ਨਕਾਬ = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_ਮਾਪ(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_ਨਕਾਬ] = rd_transparency_ਨਕਾਬ;
		cell->parameters[ID::RD_COLLISION_X_ਨਕਾਬ] = rd_collision_x_ਨਕਾਬ;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_ਮੁੱਲ] = rd_arst_ਮੁੱਲ;
		cell->parameters[ID::RD_SRST_ਮੁੱਲ] = rd_srst_ਮੁੱਲ;
		cell->parameters[ID::RD_INIT_ਮੁੱਲ] = rd_init_ਮੁੱਲ;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::RD_CLK, rd_clk);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::RD_EN, rd_en);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::RD_ARST, rd_arst);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::RD_SRST, rd_srst);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::RD_ADDR, rd_addr);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::RD_ਅੰਕੜੇ, rd_ਅੰਕੜੇ);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_ਤਰਜੀਹ_ਨਕਾਬ_builder;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : wr_ports) {
			ਲਈ (ਸਵੈਚਾਲਿਤ attr: ਪੋਰਟ.ਗੁਣ)
				ਜੇ (!cell->has_attribute(attr.ਪਹਿਲਾ))
					cell->ਗੁਣ.ਪਾਓ(attr);
			ਜੇ (ਪੋਰਟ.cell) {
				module->remove(ਪੋਰਟ.cell);
				ਪੋਰਟ.cell = ਖਾਲੀ_ਸੂਚਕ;
			}
			ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << ਪੋਰਟ.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ਧੱਕੋ_ਪਿੱਠ(State(sub != 0));
				wr_clk_enable_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਪੋਰਟ.clk_enable));
				wr_clk_polarity_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਪੋਰਟ.clk_polarity));
				wr_clk.append(ਪੋਰਟ.clk);
				ਲਈ (ਸਵੈਚਾਲਿਤ idx : wr_ਪੋਰਟ_xlat)
					wr_ਤਰਜੀਹ_ਨਕਾਬ_builder.ਧੱਕੋ_ਪਿੱਠ(State(ਤਾਰਕਿਕ(ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ[idx])));
				SigSpec addr = ਪੋਰਟ.sub_addr(sub);
				addr.extend_u0(abits, ਝੂਠ);
				wr_addr.append(addr);
				log_assert(Get_ਮਾਪ(addr) == abits);
			}
			wr_en.append(ਪੋਰਟ.en);
			wr_ਅੰਕੜੇ.append(ਪੋਰਟ.ਅੰਕੜੇ);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_ਤਰਜੀਹ_ਨਕਾਬ = wr_ਤਰਜੀਹ_ਨਕਾਬ_builder.build();
		ਜੇ (wr_ports.ਖਾਲੀ_ਥਾਵਾਂ()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_ਤਰਜੀਹ_ਨਕਾਬ = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_ਮਾਪ(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_ਤਰਜੀਹ_ਨਕਾਬ] = wr_ਤਰਜੀਹ_ਨਕਾਬ;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::WR_CLK, wr_clk);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::WR_EN, wr_en);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::WR_ADDR, wr_addr);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::WR_ਅੰਕੜੇ, wr_ਅੰਕੜੇ);
		ਲਈ (ਸਵੈਚਾਲਿਤ &init : inits) {
			ਲਈ (ਸਵੈਚਾਲਿਤ attr: init.ਗੁਣ)
				ਜੇ (!cell->has_attribute(attr.ਪਹਿਲਾ))
					cell->ਗੁਣ.ਪਾਓ(attr);
			ਜੇ (init.cell) {
				module->remove(init.cell);
				init.cell = ਖਾਲੀ_ਸੂਚਕ;
			}
		}
		cell->parameters[ID::INIT] = get_init_ਅੰਕੜੇ();
	} ਨਹੀਂਤਾਂ {
		ਜੇ (cell) {
			module->remove(cell);
			cell = ਖਾਲੀ_ਸੂਚਕ;
		}
		ਜੇ (!mem) {
			ਜੇ (memid.ਖਾਲੀ_ਥਾਵਾਂ())
				memid = PooledName(module->design, module->twines().ਜੋੜੋ(NEW_ID));
			mem = module->ਜੋੜੋ_ਸਮ੍ਰਿਤੀ(memid);
		}
		mem->ਚੌੜਾਈ = ਚੌੜਾਈ;
		mem->ਸ਼ੁਰੂ_ਵਿੱਥ = ਸ਼ੁਰੂ_ਵਿੱਥ;
		mem->ਮਾਪ = ਮਾਪ;
		mem->ਗੁਣ = ਗੁਣ;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : rd_ports) {
			ਜੇ (!ਪੋਰਟ.cell)
				ਪੋਰਟ.cell = module->ਜੋੜੋ_Cell(NEW_ID, ID($memrd_v2));
			ਪੋਰਟ.cell->type = ID($memrd_v2);
			ਪੋਰਟ.cell->ਗੁਣ = ਪੋਰਟ.ਗੁਣ;
			ਪੋਰਟ.cell->parameters[ID::MEMID] = memid.str();
			ਪੋਰਟ.cell->parameters[ID::ABITS] = Get_ਮਾਪ(ਪੋਰਟ.addr);
			ਪੋਰਟ.cell->parameters[ID::ਚੌੜਾਈ_2] = ਚੌੜਾਈ << ਪੋਰਟ.wide_log2;
			ਪੋਰਟ.cell->parameters[ID::CLK_ENABLE] = ਪੋਰਟ.clk_enable;
			ਪੋਰਟ.cell->parameters[ID::CLK_POLARITY] = ਪੋਰਟ.clk_polarity;
			ਪੋਰਟ.cell->parameters[ID::CE_OVER_SRST] = ਪੋਰਟ.ce_over_srst;
			ਪੋਰਟ.cell->parameters[ID::ARST_ਮੁੱਲ] = ਪੋਰਟ.arst_ਮੁੱਲ;
			ਪੋਰਟ.cell->parameters[ID::SRST_ਮੁੱਲ] = ਪੋਰਟ.srst_ਮੁੱਲ;
			ਪੋਰਟ.cell->parameters[ID::INIT_ਮੁੱਲ] = ਪੋਰਟ.init_ਮੁੱਲ;
			ਪੋਰਟ.cell->parameters[ID::TRANSPARENCY_ਨਕਾਬ] = ਪੋਰਟ.transparency_ਨਕਾਬ;
			ਪੋਰਟ.cell->parameters[ID::COLLISION_X_ਨਕਾਬ] = ਪੋਰਟ.collision_x_ਨਕਾਬ;
			ਪੋਰਟ.cell->parameters.erase(ID::TRANSPARENT);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, ਪੋਰਟ.clk);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, ਪੋਰਟ.en);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ARST, ਪੋਰਟ.arst);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::SRST, ਪੋਰਟ.srst);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ADDR, ਪੋਰਟ.addr);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਅੰਕੜੇ_2, ਪੋਰਟ.ਅੰਕੜੇ);
		}
		ਪੂਰਨ idx = 0;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : wr_ports) {
			ਜੇ (!ਪੋਰਟ.cell)
				ਪੋਰਟ.cell = module->ਜੋੜੋ_Cell(NEW_ID, ID($memwr_v2));
			ਪੋਰਟ.cell->type = ID($memwr_v2);
			ਪੋਰਟ.cell->ਗੁਣ = ਪੋਰਟ.ਗੁਣ;
			ਜੇ (ਪੋਰਟ.cell->parameters.ਗਿਣਤੀ_2(ID::ਤਰਜੀਹ))
				ਪੋਰਟ.cell->parameters.erase(ID::ਤਰਜੀਹ);
			ਪੋਰਟ.cell->parameters[ID::MEMID] = memid.str();
			ਪੋਰਟ.cell->parameters[ID::ABITS] = Get_ਮਾਪ(ਪੋਰਟ.addr);
			ਪੋਰਟ.cell->parameters[ID::ਚੌੜਾਈ_2] = ਚੌੜਾਈ << ਪੋਰਟ.wide_log2;
			ਪੋਰਟ.cell->parameters[ID::CLK_ENABLE] = ਪੋਰਟ.clk_enable;
			ਪੋਰਟ.cell->parameters[ID::CLK_POLARITY] = ਪੋਰਟ.clk_polarity;
			ਪੋਰਟ.cell->parameters[ID::PORTID] = idx++;
			ਪੋਰਟ.cell->parameters[ID::ਤਰਜੀਹ_ਨਕਾਬ_2] = ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ;
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, ਪੋਰਟ.clk);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, ਪੋਰਟ.en);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ADDR, ਪੋਰਟ.addr);
			ਪੋਰਟ.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਅੰਕੜੇ_2, ਪੋਰਟ.ਅੰਕੜੇ);
		}
		idx = 0;
		ਲਈ (ਸਵੈਚਾਲਿਤ &init : inits) {
			ਤਾਰਕਿਕ v2 = !init.en.is_fully_ones();
			ਜੇ (!init.cell)
				init.cell = module->ਜੋੜੋ_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			ਨਹੀਂਤਾਂ {
				ਜੇ (!v2)
					init.cell->unset_ਪੋਰਟ(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->ਗੁਣ = init.ਗੁਣ;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_ਮਾਪ(init.addr);
			init.cell->parameters[ID::ਚੌੜਾਈ_2] = ਚੌੜਾਈ;
			init.cell->parameters[ID::ਸ਼ਬਦ_2] = Get_ਮਾਪ(init.ਅੰਕੜੇ) / ਚੌੜਾਈ;
			init.cell->parameters[ID::ਤਰਜੀਹ] = idx++;
			init.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ADDR, init.addr);
			init.cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਅੰਕੜੇ_2, init.ਅੰਕੜੇ);
			ਜੇ (v2)
				init.cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, init.en);
		}
	}
}

ਖਾਲੀ Mem::clear_inits() {
	ਲਈ (ਸਵੈਚਾਲਿਤ &init : inits)
		init.removed = ਸੱਚ;
}

ਖਾਲੀ Mem::coalesce_inits() {
	// start address -> end address
	std::map<ਪੂਰਨ, ਪੂਰਨ> chunks;
	// Figure out chunk boundaries.
	ਲਈ (ਸਵੈਚਾਲਿਤ &init : inits) {
		ਜੇ (init.removed)
			ਜਾਰੀ;
		ਤਾਰਕਿਕ valid = ਝੂਠ;
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : init.en)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::S1)
				valid = ਸੱਚ;
		ਜੇ (!valid) {
			init.removed = ਸੱਚ;
			ਜਾਰੀ;
		}
		ਪੂਰਨ addr = init.addr.as_int();
		ਪੂਰਨ addr_e = addr + Get_ਮਾਪ(init.ਅੰਕੜੇ) / ਚੌੜਾਈ;
		ਸਵੈਚਾਲਿਤ it_e = chunks.ਉੱਤਲਾ_bound(addr_e);
		ਸਵੈਚਾਲਿਤ it = it_e;
		ਜਦੋਂ (it != chunks.begin()) {
			--it;
			ਜੇ (it->ਦੂਜਾ < addr) {
				++it;
				ਰੋਕੋ;
			}
		}
		ਜੇ (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} ਨਹੀਂਤਾਂ {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			ਜੇ (it->ਪਹਿਲਾ < addr)
				addr = it->ਪਹਿਲਾ;
			ਸਵੈਚਾਲਿਤ it_ਆਖਰੀ = it_e;
			it_ਆਖਰੀ--;
			ਜੇ (it_ਆਖਰੀ->ਦੂਜਾ > addr_e)
				addr_e = it_ਆਖਰੀ->ਦੂਜਾ;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<ਪੂਰਨ, std::ਸਦਿਸ਼<ਪੂਰਨ>> inits_by_chunk;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(inits); i++) {
		ਸਵੈਚਾਲਿਤ &init = inits[i];
		ਜੇ (init.removed)
			ਜਾਰੀ;
		ਸਵੈਚਾਲਿਤ it = chunks.ਉੱਤਲਾ_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->ਪਹਿਲਾ].ਧੱਕੋ_ਪਿੱਠ(i);
		ਪੂਰਨ addr = init.addr.as_int();
		ਪੂਰਨ addr_e = addr + Get_ਮਾਪ(init.ਅੰਕੜੇ) / ਚੌੜਾਈ;
		log_assert(addr >= it->ਪਹਿਲਾ && addr_e <= it->ਦੂਜਾ);
	}
	// Process each chunk.
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : inits_by_chunk) {
		ਪੂਰਨ caddr = it.ਪਹਿਲਾ;
		ਪੂਰਨ caddr_e = chunks[caddr];
		ਸਵੈਚਾਲਿਤ &chunk_inits = it.ਦੂਜਾ;
		ਜੇ (Get_ਮਾਪ(chunk_inits) == 1) {
			ਸਵੈਚਾਲਿਤ &init = inits[chunk_inits[0]];
			ਜੇ (!init.en.is_fully_ones()) {
				ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(init.ਅੰਕੜੇ); i++)
					ਜੇ (init.en[i % ਚੌੜਾਈ] != State::S1)
						init.ਅੰਕੜੇ.ਸੈੱਟ(i, State::Sx);
				init.en = Const(State::S1, ਚੌੜਾਈ);
			}
			ਜਾਰੀ;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * ਚੌੜਾਈ);
		ਲਈ (ਪੂਰਨ idx : chunk_inits) {
			ਸਵੈਚਾਲਿਤ &init = inits[idx];
			ਪੂਰਨ ਵਿੱਥ = (init.addr.as_int() - caddr) * ਚੌੜਾਈ;
			log_assert(ਵਿੱਥ >= 0);
			log_assert(ਵਿੱਥ + Get_ਮਾਪ(init.ਅੰਕੜੇ) <= Get_ਮਾਪ(cdata));
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(init.ਅੰਕੜੇ); i++)
				ਜੇ (init.en[i % ਚੌੜਾਈ] == State::S1)
					cdata.ਸੈੱਟ(i+ਵਿੱਥ, init.ਅੰਕੜੇ[i]);
			init.removed = ਸੱਚ;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.ਅੰਕੜੇ = cdata;
		new_init.en = Const(State::S1, ਚੌੜਾਈ);
		inits.ਧੱਕੋ_ਪਿੱਠ(new_init);
	}
}

Const Mem::get_init_ਅੰਕੜੇ() ਅਚਲ {
	Const init_ਅੰਕੜੇ(State::Sx, ਚੌੜਾਈ * ਮਾਪ);
	ਲਈ (ਸਵੈਚਾਲਿਤ &init : inits) {
		ਜੇ (init.removed)
			ਜਾਰੀ;
		ਪੂਰਨ ਵਿੱਥ = (init.addr.as_int() - ਸ਼ੁਰੂ_ਵਿੱਥ) * ਚੌੜਾਈ;
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(init.ਅੰਕੜੇ); i++)
			ਜੇ (0 <= i+ਵਿੱਥ && i+ਵਿੱਥ < Get_ਮਾਪ(init_ਅੰਕੜੇ) && init.en[i % ਚੌੜਾਈ] == State::S1)
				init_ਅੰਕੜੇ.ਸੈੱਟ(i+ਵਿੱਥ, init.ਅੰਕੜੇ[i]);
	}
	ਵਾਪਸ init_ਅੰਕੜੇ;
}

ਖਾਲੀ Mem::check() {
	ਪੂਰਨ max_wide_log2 = 0;
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : rd_ports) {
		ਜੇ (ਪੋਰਟ.removed)
			ਜਾਰੀ;
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.clk) == 1);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.en) == 1);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.arst) == 1);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.srst) == 1);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.addr) >= ਪੋਰਟ.wide_log2);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ) == (ਚੌੜਾਈ << ਪੋਰਟ.wide_log2));
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.init_ਮੁੱਲ) == (ਚੌੜਾਈ << ਪੋਰਟ.wide_log2));
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.arst_ਮੁੱਲ) == (ਚੌੜਾਈ << ਪੋਰਟ.wide_log2));
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.srst_ਮੁੱਲ) == (ਚੌੜਾਈ << ਪੋਰਟ.wide_log2));
		ਜੇ (!ਪੋਰਟ.clk_enable) {
			log_assert(ਪੋਰਟ.en == State::S1);
			log_assert(ਪੋਰਟ.arst == State::S0);
			log_assert(ਪੋਰਟ.srst == State::S0);
		}
		ਲਈ (ਪੂਰਨ j = 0; j < ਪੋਰਟ.wide_log2; j++) {
			log_assert(ਪੋਰਟ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, ਪੋਰਟ.wide_log2);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.transparency_ਨਕਾਬ) == Get_ਮਾਪ(wr_ports));
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.collision_x_ਨਕਾਬ) == Get_ਮਾਪ(wr_ports));
		ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(wr_ports); j++) {
			ਸਵੈਚਾਲਿਤ &wport = wr_ports[j];
			ਜੇ ((ਪੋਰਟ.transparency_ਨਕਾਬ[j] || ਪੋਰਟ.collision_x_ਨਕਾਬ[j]) && !wport.removed) {
				log_assert(ਪੋਰਟ.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(ਪੋਰਟ.clk == wport.clk);
				log_assert(ਪੋਰਟ.clk_polarity == wport.clk_polarity);
			}
			log_assert(!ਪੋਰਟ.transparency_ਨਕਾਬ[j] || !ਪੋਰਟ.collision_x_ਨਕਾਬ[j]);
		}
	}
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++) {
		ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = wr_ports[i];
		ਜੇ (ਪੋਰਟ.removed)
			ਜਾਰੀ;
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.clk) == 1);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.en) == (ਚੌੜਾਈ << ਪੋਰਟ.wide_log2));
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ) == (ਚੌੜਾਈ << ਪੋਰਟ.wide_log2));
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.addr) >= ਪੋਰਟ.wide_log2);
		ਲਈ (ਪੂਰਨ j = 0; j < ਪੋਰਟ.wide_log2; j++) {
			log_assert(ਪੋਰਟ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, ਪੋਰਟ.wide_log2);
		log_assert(Get_ਮਾਪ(ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ) == Get_ਮਾਪ(wr_ports));
		ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(wr_ports); j++) {
			ਸਵੈਚਾਲਿਤ &wport = wr_ports[j];
			ਜੇ (ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(ਪੋਰਟ.clk_enable == wport.clk_enable);
				ਜੇ (ਪੋਰਟ.clk_enable) {
					log_assert(ਪੋਰਟ.clk == wport.clk);
					log_assert(ਪੋਰਟ.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	ਪੂਰਨ ਨਕਾਬ = (1 << max_wide_log2) - 1;
	log_assert(!(ਸ਼ੁਰੂ_ਵਿੱਥ & ਨਕਾਬ));
	log_assert(!(ਮਾਪ & ਨਕਾਬ));
	log_assert(ਚੌੜਾਈ != 0);
}

ਨਾਮਾਕਾਸ਼ {

	ਬਣਤਰ Mem_ਸੂਚਕਾਂਕ {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_ਸੂਚਕਾਂਕ (Module *module) {
			ਲਈ (ਸਵੈਚਾਲਿਤ cell: module->cells()) {
				ਜੇ (cell->type.ਵਿੱਚ(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].ਪਾਓ(cell);
				ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].ਪਾਓ(cell);
				ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].ਪਾਓ(cell);
			}
		}
	};

	Mem mem_from_ਸਮ੍ਰਿਤੀ(Module *module, RTLIL::ਸਮ੍ਰਿਤੀ *mem, ਅਚਲ Mem_ਸੂਚਕਾਂਕ &ਸੂਚਕਾਂਕ) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->ਚੌੜਾਈ, mem->ਸ਼ੁਰੂ_ਵਿੱਥ, mem->ਮਾਪ);
		res.packed = ਝੂਠ;
		res.mem = mem;
		res.ਗੁਣ = mem->ਗੁਣ;
		std::ਸਦਿਸ਼<ਤਾਰਕਿਕ> rd_transparent;
		std::ਸਦਿਸ਼<ਪੂਰਨ> wr_portid;
		ਜੇ (ਸੂਚਕਾਂਕ.rd_ports.ਗਿਣਤੀ_2(memid)) {
			ਲਈ (ਸਵੈਚਾਲਿਤ cell : ਸੂਚਕਾਂਕ.rd_ports.at(memid)) {
				MemRd mrd;
				ਤਾਰਕਿਕ is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.ਗੁਣ = cell->ਗੁਣ;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_ਪੋਰਟ(ID::CLK);
				mrd.en = cell->get_ਪੋਰਟ(ID::EN);
				mrd.addr = cell->get_ਪੋਰਟ(ID::ADDR);
				mrd.ਅੰਕੜੇ = cell->get_ਪੋਰਟ(ID::ਅੰਕੜੇ_2);
				mrd.wide_log2 = ceil_log2(Get_ਮਾਪ(mrd.ਅੰਕੜੇ) / mem->ਚੌੜਾਈ);
				ਤਾਰਕਿਕ transparent = ਝੂਠ;
				ਜੇ (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = ਝੂਠ;
					mrd.arst_ਮੁੱਲ = Const(State::Sx, mem->ਚੌੜਾਈ << mrd.wide_log2);
					mrd.srst_ਮੁੱਲ = Const(State::Sx, mem->ਚੌੜਾਈ << mrd.wide_log2);
					mrd.init_ਮੁੱਲ = Const(State::Sx, mem->ਚੌੜਾਈ << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					ਜੇ (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = ਝੂਠ;
						ਜੇ (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} ਨਹੀਂਤਾਂ {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_ਮੁੱਲ = cell->parameters.at(ID::ARST_ਮੁੱਲ);
					mrd.srst_ਮੁੱਲ = cell->parameters.at(ID::SRST_ਮੁੱਲ);
					mrd.init_ਮੁੱਲ = cell->parameters.at(ID::INIT_ਮੁੱਲ);
					mrd.arst = cell->get_ਪੋਰਟ(ID::ARST);
					mrd.srst = cell->get_ਪੋਰਟ(ID::SRST);
				}
				res.rd_ports.ਧੱਕੋ_ਪਿੱਠ(mrd);
				rd_transparent.ਧੱਕੋ_ਪਿੱਠ(transparent);
			}
		}
		ਜੇ (ਸੂਚਕਾਂਕ.wr_ports.ਗਿਣਤੀ_2(memid)) {
			std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, MemWr>> ports;
			ਲਈ (ਸਵੈਚਾਲਿਤ cell : ਸੂਚਕਾਂਕ.wr_ports.at(memid)) {
				MemWr mwr;
				ਤਾਰਕਿਕ is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.ਗੁਣ = cell->ਗੁਣ;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_ਪੋਰਟ(ID::CLK);
				mwr.en = cell->get_ਪੋਰਟ(ID::EN);
				mwr.addr = cell->get_ਪੋਰਟ(ID::ADDR);
				mwr.ਅੰਕੜੇ = cell->get_ਪੋਰਟ(ID::ਅੰਕੜੇ_2);
				mwr.wide_log2 = ceil_log2(Get_ਮਾਪ(mwr.ਅੰਕੜੇ) / mem->ਚੌੜਾਈ);
				ports.ਧੱਕੋ_ਪਿੱਠ(std::make_pair(cell->parameters.at(is_compat ? ID::ਤਰਜੀਹ : ID::PORTID).as_int(), mwr));
			}
			std::ਛਾਂਟੋ(ports.begin(), ports.ਸਿਰਾ(), [](ਅਚਲ std::pair<ਪੂਰਨ, MemWr> &a, ਅਚਲ std::pair<ਪੂਰਨ, MemWr> &b) { ਵਾਪਸ a.ਪਹਿਲਾ < b.ਪਹਿਲਾ; });
			ਲਈ (ਸਵੈਚਾਲਿਤ &it : ports) {
				res.wr_ports.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
				wr_portid.ਧੱਕੋ_ਪਿੱਠ(it.ਪਹਿਲਾ);
			}
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(res.wr_ports); i++) {
				ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = res.wr_ports[i];
				ਤਾਰਕਿਕ is_compat = ਪੋਰਟ.cell->type == ID($memwr);
				ਜੇ (is_compat) {
					ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ.resize(Get_ਮਾਪ(res.wr_ports));
					ਲਈ (ਪੂਰਨ j = 0; j < i; j++) {
						ਸਵੈਚਾਲਿਤ &oport = res.wr_ports[j];
						ਜੇ (ਪੋਰਟ.clk_enable != oport.clk_enable)
							ਜਾਰੀ;
						ਜੇ (ਪੋਰਟ.clk_enable && ਪੋਰਟ.clk != oport.clk)
							ਜਾਰੀ;
						ਜੇ (ਪੋਰਟ.clk_enable && ਪੋਰਟ.clk_polarity != oport.clk_polarity)
							ਜਾਰੀ;
						ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ[j] = ਸੱਚ;
					}
				} ਨਹੀਂਤਾਂ {
					Const orig_prio_ਨਕਾਬ = ਪੋਰਟ.cell->parameters.at(ID::ਤਰਜੀਹ_ਨਕਾਬ_2);
					ਲਈ (ਪੂਰਨ orig_portid : wr_portid) {
						ਤਾਰਕਿਕ has_prio = orig_portid < Get_ਮਾਪ(orig_prio_ਨਕਾਬ) && orig_prio_ਨਕਾਬ[orig_portid] == State::S1;
						ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(has_prio);
					}
				}
			}
		}
		ਜੇ (ਸੂਚਕਾਂਕ.inits.ਗਿਣਤੀ_2(memid)) {
			std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, MemInit>> inits;
			ਲਈ (ਸਵੈਚਾਲਿਤ cell : ਸੂਚਕਾਂਕ.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.ਗੁਣ = cell->ਗੁਣ;
				ਸਵੈਚਾਲਿਤ addr = cell->get_ਪੋਰਟ(ID::ADDR);
				ਸਵੈਚਾਲਿਤ ਅੰਕੜੇ = cell->get_ਪੋਰਟ(ID::ਅੰਕੜੇ_2);
				ਜੇ (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				ਜੇ (!ਅੰਕੜੇ.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(ਅੰਕੜੇ), cell);
				init.addr = addr.as_const();
				init.ਅੰਕੜੇ = ਅੰਕੜੇ.as_const();
				ਜੇ (cell->type == ID($meminit_v2)) {
					ਸਵੈਚਾਲਿਤ en = cell->get_ਪੋਰਟ(ID::EN);
					ਜੇ (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} ਨਹੀਂਤਾਂ {
					init.en = RTLIL::Const(State::S1, mem->ਚੌੜਾਈ);
				}
				inits.ਧੱਕੋ_ਪਿੱਠ(std::make_pair(cell->parameters.at(ID::ਤਰਜੀਹ).as_int(), init));
			}
			std::ਛਾਂਟੋ(inits.begin(), inits.ਸਿਰਾ(), [](ਅਚਲ std::pair<ਪੂਰਨ, MemInit> &a, ਅਚਲ std::pair<ਪੂਰਨ, MemInit> &b) { ਵਾਪਸ a.ਪਹਿਲਾ < b.ਪਹਿਲਾ; });
			ਲਈ (ਸਵੈਚਾਲਿਤ &it : inits)
				res.inits.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
		}
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(res.rd_ports); i++) {
			ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = res.rd_ports[i];
			ਤਾਰਕਿਕ is_compat = ਪੋਰਟ.cell->type == ID($memrd);
			ਜੇ (is_compat) {
				ਪੋਰਟ.transparency_ਨਕਾਬ.resize(Get_ਮਾਪ(res.wr_ports));
				ਪੋਰਟ.collision_x_ਨਕਾਬ.resize(Get_ਮਾਪ(res.wr_ports));
				ਜੇ (!rd_transparent[i])
					ਜਾਰੀ;
				ਜੇ (!ਪੋਰਟ.clk_enable)
					ਜਾਰੀ;
				ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(res.wr_ports); j++) {
					ਸਵੈਚਾਲਿਤ &wport = res.wr_ports[j];
					ਜੇ (!wport.clk_enable)
						ਜਾਰੀ;
					ਜੇ (ਪੋਰਟ.clk != wport.clk)
						ਜਾਰੀ;
					ਜੇ (ਪੋਰਟ.clk_polarity != wport.clk_polarity)
						ਜਾਰੀ;
					ਪੋਰਟ.transparency_ਨਕਾਬ[j] = ਸੱਚ;
				}
			} ਨਹੀਂਤਾਂ {
				Const orig_trans_ਨਕਾਬ = ਪੋਰਟ.cell->parameters.at(ID::TRANSPARENCY_ਨਕਾਬ);
				Const orig_cx_ਨਕਾਬ = ਪੋਰਟ.cell->parameters.at(ID::COLLISION_X_ਨਕਾਬ);
				ਲਈ (ਪੂਰਨ orig_portid : wr_portid) {
					ਪੋਰਟ.transparency_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(orig_portid < Get_ਮਾਪ(orig_trans_ਨਕਾਬ) && orig_trans_ਨਕਾਬ[orig_portid] == State::S1);
					ਪੋਰਟ.collision_x_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(orig_portid < Get_ਮਾਪ(orig_cx_ਨਕਾਬ) && orig_cx_ਨਕਾਬ[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		ਵਾਪਸ res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().ਜੋੜੋ(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::ਚੌੜਾਈ_2).as_int(),
			cell->parameters.at(ID::ਵਿੱਥ_2).as_int(),
			cell->parameters.at(ID::ਮਾਪ_2).as_int()
		);
		ਤਾਰਕਿਕ is_compat = cell->type == ID($mem);
		ਪੂਰਨ abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = ਸੱਚ;
		res.cell = cell;
		res.ਗੁਣ = cell->ਗੁਣ;
		Const &init = cell->parameters.at(ID::INIT);
		ਜੇ (!init.is_fully_undef()) {
			ਪੂਰਨ pos = 0;
			ਜਦੋਂ (pos < res.ਮਾਪ) {
				Const ਸ਼ਬਦ = init.ਕੱਢੋ_2(pos * res.ਚੌੜਾਈ, res.ਚੌੜਾਈ, State::Sx);
				ਜੇ (ਸ਼ਬਦ.is_fully_undef()) {
					pos++;
				} ਨਹੀਂਤਾਂ {
					ਪੂਰਨ epos;
					ਲਈ (epos = pos; epos < res.ਮਾਪ; epos++) {
						Const eword = init.ਕੱਢੋ_2(epos * res.ਚੌੜਾਈ, res.ਚੌੜਾਈ, State::Sx);
						ਜੇ (eword.is_fully_undef())
							ਰੋਕੋ;
					}
					MemInit minit;
					minit.addr = res.ਸ਼ੁਰੂ_ਵਿੱਥ + pos;
					minit.ਅੰਕੜੇ = init.ਕੱਢੋ_2(pos * res.ਚੌੜਾਈ, (epos - pos) * res.ਚੌੜਾਈ, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.ਚੌੜਾਈ);
					res.inits.ਧੱਕੋ_ਪਿੱਠ(minit);
					pos = epos;
				}
			}
		}
		ਪੂਰਨ n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		ਪੂਰਨ n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		ਲਈ (ਪੂਰਨ i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			ਜਦੋਂ (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).ਕੱਢੋ_2(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).ਕੱਢੋ_2(i, 1).as_bool();
			mrd.clk = cell->get_ਪੋਰਟ(ID::RD_CLK).ਕੱਢੋ_2(i, 1);
			mrd.en = cell->get_ਪੋਰਟ(ID::RD_EN).ਕੱਢੋ_2(i, 1);
			mrd.addr = cell->get_ਪੋਰਟ(ID::RD_ADDR).ਕੱਢੋ_2(i * abits, abits);
			mrd.ਅੰਕੜੇ = cell->get_ਪੋਰਟ(ID::RD_ਅੰਕੜੇ).ਕੱਢੋ_2(i * res.ਚੌੜਾਈ, (ni - i) * res.ਚੌੜਾਈ);
			ਜੇ (is_compat) {
				mrd.ce_over_srst = ਝੂਠ;
				mrd.arst_ਮੁੱਲ = Const(State::Sx, res.ਚੌੜਾਈ << mrd.wide_log2);
				mrd.srst_ਮੁੱਲ = Const(State::Sx, res.ਚੌੜਾਈ << mrd.wide_log2);
				mrd.init_ਮੁੱਲ = Const(State::Sx, res.ਚੌੜਾਈ << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} ਨਹੀਂਤਾਂ {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).ਕੱਢੋ_2(i, 1).as_bool();
				mrd.arst_ਮੁੱਲ = cell->parameters.at(ID::RD_ARST_ਮੁੱਲ).ਕੱਢੋ_2(i * res.ਚੌੜਾਈ, (ni - i) * res.ਚੌੜਾਈ);
				mrd.srst_ਮੁੱਲ = cell->parameters.at(ID::RD_SRST_ਮੁੱਲ).ਕੱਢੋ_2(i * res.ਚੌੜਾਈ, (ni - i) * res.ਚੌੜਾਈ);
				mrd.init_ਮੁੱਲ = cell->parameters.at(ID::RD_INIT_ਮੁੱਲ).ਕੱਢੋ_2(i * res.ਚੌੜਾਈ, (ni - i) * res.ਚੌੜਾਈ);
				mrd.arst = cell->get_ਪੋਰਟ(ID::RD_ARST).ਕੱਢੋ_2(i, 1);
				mrd.srst = cell->get_ਪੋਰਟ(ID::RD_SRST).ਕੱਢੋ_2(i, 1);
			}
			ਜੇ (!is_compat) {
				Const transparency_ਨਕਾਬ = cell->parameters.at(ID::RD_TRANSPARENCY_ਨਕਾਬ).ਕੱਢੋ_2(i * n_wr_ports, n_wr_ports);
				Const collision_x_ਨਕਾਬ = cell->parameters.at(ID::RD_COLLISION_X_ਨਕਾਬ).ਕੱਢੋ_2(i * n_wr_ports, n_wr_ports);
				ਲਈ (ਪੂਰਨ j = 0; j < n_wr_ports; j++)
					ਜੇ (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(transparency_ਨਕਾਬ[j] == State::S1);
						mrd.collision_x_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(collision_x_ਨਕਾਬ[j] == State::S1);
					}
			}
			res.rd_ports.ਧੱਕੋ_ਪਿੱਠ(mrd);
		}
		ਲਈ (ਪੂਰਨ i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			ਜਦੋਂ (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).ਕੱਢੋ_2(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).ਕੱਢੋ_2(i, 1).as_bool();
			mwr.clk = cell->get_ਪੋਰਟ(ID::WR_CLK).ਕੱਢੋ_2(i, 1);
			mwr.en = cell->get_ਪੋਰਟ(ID::WR_EN).ਕੱਢੋ_2(i * res.ਚੌੜਾਈ, (ni - i) * res.ਚੌੜਾਈ);
			mwr.addr = cell->get_ਪੋਰਟ(ID::WR_ADDR).ਕੱਢੋ_2(i * abits, abits);
			mwr.ਅੰਕੜੇ = cell->get_ਪੋਰਟ(ID::WR_ਅੰਕੜੇ).ਕੱਢੋ_2(i * res.ਚੌੜਾਈ, (ni - i) * res.ਚੌੜਾਈ);
			ਜੇ (!is_compat) {
				Const ਤਰਜੀਹ_ਨਕਾਬ = cell->parameters.at(ID::WR_ਤਰਜੀਹ_ਨਕਾਬ).ਕੱਢੋ_2(i * n_wr_ports, n_wr_ports);
				ਲਈ (ਪੂਰਨ j = 0; j < n_wr_ports; j++)
					ਜੇ (wr_wide_continuation[j] != State::S1)
						mwr.ਤਰਜੀਹ_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(ਤਰਜੀਹ_ਨਕਾਬ[j] == State::S1);
			}
			res.wr_ports.ਧੱਕੋ_ਪਿੱਠ(mwr);
		}
		ਜੇ (is_compat) {
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(res.wr_ports); i++) {
				ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = res.wr_ports[i];
				ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ.resize(Get_ਮਾਪ(res.wr_ports));
				ਲਈ (ਪੂਰਨ j = 0; j < i; j++) {
					ਸਵੈਚਾਲਿਤ &oport = res.wr_ports[j];
					ਜੇ (ਪੋਰਟ.clk_enable != oport.clk_enable)
						ਜਾਰੀ;
					ਜੇ (ਪੋਰਟ.clk_enable && ਪੋਰਟ.clk != oport.clk)
						ਜਾਰੀ;
					ਜੇ (ਪੋਰਟ.clk_enable && ਪੋਰਟ.clk_polarity != oport.clk_polarity)
						ਜਾਰੀ;
					ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ[j] = ਸੱਚ;
				}
			}
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(res.rd_ports); i++) {
				ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = res.rd_ports[i];
				ਪੋਰਟ.transparency_ਨਕਾਬ.resize(Get_ਮਾਪ(res.wr_ports));
				ਪੋਰਟ.collision_x_ਨਕਾਬ.resize(Get_ਮਾਪ(res.wr_ports));
				ਜੇ (!cell->parameters.at(ID::RD_TRANSPARENT).ਕੱਢੋ_2(i, 1).as_bool())
					ਜਾਰੀ;
				ਜੇ (!ਪੋਰਟ.clk_enable)
					ਜਾਰੀ;
				ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(res.wr_ports); j++) {
					ਸਵੈਚਾਲਿਤ &wport = res.wr_ports[j];
					ਜੇ (!wport.clk_enable)
						ਜਾਰੀ;
					ਜੇ (ਪੋਰਟ.clk != wport.clk)
						ਜਾਰੀ;
					ਜੇ (ਪੋਰਟ.clk_polarity != wport.clk_polarity)
						ਜਾਰੀ;
					ਪੋਰਟ.transparency_ਨਕਾਬ[j] = ਸੱਚ;
				}
			}
		}
		res.check();
		ਵਾਪਸ res;
	}

}

std::ਸਦਿਸ਼<Mem> Mem::get_all_memories(Module *module) {
	std::ਸਦਿਸ਼<Mem> res;
	Mem_ਸੂਚਕਾਂਕ ਸੂਚਕਾਂਕ(module);
	ਲਈ (ਸਵੈਚਾਲਿਤ it: module->memories) {
		res.ਧੱਕੋ_ਪਿੱਠ(mem_from_ਸਮ੍ਰਿਤੀ(module, it.ਦੂਜਾ, ਸੂਚਕਾਂਕ));
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ cell: module->cells()) {
		ਜੇ (cell->type.ਵਿੱਚ(ID($mem), ID($mem_v2)))
			res.ਧੱਕੋ_ਪਿੱਠ(mem_from_cell(cell));
	}
	ਵਾਪਸ res;
}

std::ਸਦਿਸ਼<Mem> Mem::get_selected_memories(Module *module) {
	std::ਸਦਿਸ਼<Mem> res;
	Mem_ਸੂਚਕਾਂਕ ਸੂਚਕਾਂਕ(module);
	ਲਈ (ਸਵੈਚਾਲਿਤ it: module->memories) {
		ਜੇ (module->design->selected(module, it.ਦੂਜਾ))
			res.ਧੱਕੋ_ਪਿੱਠ(mem_from_ਸਮ੍ਰਿਤੀ(module, it.ਦੂਜਾ, ਸੂਚਕਾਂਕ));
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ cell: module->selected_cells()) {
		ਜੇ (cell->type.ਵਿੱਚ(ID($mem), ID($mem_v2)))
			res.ਧੱਕੋ_ਪਿੱਠ(mem_from_cell(cell));
	}
	ਵਾਪਸ res;
}

Cell *Mem::ਕੱਢੋ_rdff(ਪੂਰਨ idx, FfInitVals *initvals) {
	MemRd &ਪੋਰਟ = rd_ports[idx];

	ਜੇ (!ਪੋਰਟ.clk_enable)
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

	log_assert(module && module->design);
	std::string memid_str = memid.str();

	Cell *c;

	// There are two ways to handle rdff extraction when transparency is involved:
	//
	// - if all of the following conditions are true, put the FF on address input:
	//
	//   - the port has no clock enable, no reset, and no initial value
	//   - the port is transparent wrt all write ports (implying they also share
	//     the clock domain)
	//
	// - otherwise, put the FF on the data output, and make bypass paths for
	//   all write ports wrt which this port is transparent
	ਤਾਰਕਿਕ trans_use_addr = ਸੱਚ;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++)
		ਜੇ (!ਪੋਰਟ.transparency_ਨਕਾਬ[i] && !wr_ports[i].removed)
			trans_use_addr = ਝੂਠ;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	ਜੇ (Get_ਮਾਪ(wr_ports) == 0)
		trans_use_addr = ਝੂਠ;

	ਜੇ (ਪੋਰਟ.en != State::S1 || ਪੋਰਟ.srst != State::S0 || ਪੋਰਟ.arst != State::S0 || !ਪੋਰਟ.init_ਮੁੱਲ.is_fully_undef())
		trans_use_addr = ਝੂਠ;

	ਜੇ (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		ਪੂਰਨ ਚੌੜਾਈ = 0;
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(ਪੋਰਟ.addr); i++)
			ਜੇ (ਪੋਰਟ.addr[i].wire)
				ਚੌੜਾਈ++;

		ਜੇ (ਚੌੜਾਈ)
		{
			SigSpec sig_q = module->ਜੋੜੋ_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), ਚੌੜਾਈ);
			SigSpec sig_d;

			ਪੂਰਨ pos = 0;
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(ਪੋਰਟ.addr); i++)
				ਜੇ (ਪੋਰਟ.addr[i].wire) {
					sig_d.append(ਪੋਰਟ.addr[i]);
					ਪੋਰਟ.addr[i] = sig_q[pos++];
				}

			c = module->ਜੋੜੋ_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), ਪੋਰਟ.clk, sig_d, sig_q, ਪੋਰਟ.clk_polarity);
		} ਨਹੀਂਤਾਂ {
			c = ਖਾਲੀ_ਸੂਚਕ;
		}
	}
	ਨਹੀਂਤਾਂ
	{
		log_assert(ਪੋਰਟ.arst == State::S0 || ਪੋਰਟ.srst == State::S0);

		SigSpec async_d = module->ਜੋੜੋ_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
		SigSpec sig_d = async_d;

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++) {
			ਸਵੈਚਾਲਿਤ &wport = wr_ports[i];
			ਜੇ (wport.removed)
				ਜਾਰੀ;
			ਜੇ (ਪੋਰਟ.transparency_ਨਕਾਬ[i] || ਪੋਰਟ.collision_x_ਨਕਾਬ[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == ਪੋਰਟ.clk);
				log_assert(wport.clk_enable == ਪੋਰਟ.clk_enable);
				ਪੂਰਨ min_wide_log2 = std::min(ਪੋਰਟ.wide_log2, wport.wide_log2);
				ਪੂਰਨ max_wide_log2 = std::max(ਪੋਰਟ.wide_log2, wport.wide_log2);
				ਤਾਰਕਿਕ wide_write = wport.wide_log2 > ਪੋਰਟ.wide_log2;
				ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = ਪੋਰਟ.addr;
					SigSpec waddr = wport.addr;
					ਜੇ (wide_write)
						waddr = wport.sub_addr(sub);
					ਨਹੀਂਤਾਂ
						raddr = ਪੋਰਟ.sub_addr(sub);
					SigSpec addr_eq;
					ਜੇ (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					ਪੂਰਨ pos = 0;
					ਪੂਰਨ ewidth = ਚੌੜਾਈ << min_wide_log2;
					ਪੂਰਨ wsub = wide_write ? sub : 0;
					ਪੂਰਨ rsub = wide_write ? 0 : sub;
					ਜਦੋਂ (pos < ewidth) {
						ਪੂਰਨ epos = pos;
						ਜਦੋਂ (epos < ewidth && wport.en[epos + wsub * ਚੌੜਾਈ] == wport.en[pos + wsub * ਚੌੜਾਈ])
							epos++;
						SigSpec cur = sig_d.ਕੱਢੋ_2(pos + rsub * ਚੌੜਾਈ, epos-pos);
						SigSpec other = ਪੋਰਟ.transparency_ਨਕਾਬ[i] ? wport.ਅੰਕੜੇ.ਕੱਢੋ_2(pos + wsub * ਚੌੜਾਈ, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						ਜੇ (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * ਚੌੜਾਈ], addr_eq);
						ਨਹੀਂਤਾਂ
							cond = wport.en[pos + wsub * ਚੌੜਾਈ];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * ਚੌੜਾਈ, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().ਜੋੜੋ(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_ਅੰਕੜੇ ff(module, initvals, name);
		ff.ਚੌੜਾਈ = Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ);
		ff.has_clk = ਸੱਚ;
		ff.sig_clk = ਪੋਰਟ.clk;
		ff.pol_clk = ਪੋਰਟ.clk_polarity;
		ਜੇ (ਪੋਰਟ.en != State::S1) {
			ff.has_ce = ਸੱਚ;
			ff.pol_ce = ਸੱਚ;
			ff.sig_ce = ਪੋਰਟ.en;
		}
		ਜੇ (ਪੋਰਟ.arst != State::S0) {
			ff.has_arst = ਸੱਚ;
			ff.pol_arst = ਸੱਚ;
			ff.sig_arst = ਪੋਰਟ.arst;
			ff.val_arst = ਪੋਰਟ.arst_ਮੁੱਲ;
		}
		ਜੇ (ਪੋਰਟ.srst != State::S0) {
			ff.has_srst = ਸੱਚ;
			ff.pol_srst = ਸੱਚ;
			ff.sig_srst = ਪੋਰਟ.srst;
			ff.val_srst = ਪੋਰਟ.srst_ਮੁੱਲ;
			ff.ce_over_srst = ff.has_ce && ਪੋਰਟ.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = ਪੋਰਟ.ਅੰਕੜੇ;
		ff.val_init = ਪੋਰਟ.init_ਮੁੱਲ;
		ਪੋਰਟ.ਅੰਕੜੇ = async_d;
		c = ff.emit();
	}

	ਜੇ (c)
		ਲਘੂਗਣਕ("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	ਪੋਰਟ.en = State::S1;
	ਪੋਰਟ.clk = State::S0;
	ਪੋਰਟ.arst = State::S0;
	ਪੋਰਟ.srst = State::S0;
	ਪੋਰਟ.clk_enable = ਝੂਠ;
	ਪੋਰਟ.clk_polarity = ਸੱਚ;
	ਪੋਰਟ.ce_over_srst = ਝੂਠ;
	ਪੋਰਟ.arst_ਮੁੱਲ = Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	ਪੋਰਟ.srst_ਮੁੱਲ = Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	ਪੋਰਟ.init_ਮੁੱਲ = Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));

	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++) {
		ਪੋਰਟ.transparency_ਨਕਾਬ[i] = ਝੂਠ;
		ਪੋਰਟ.collision_x_ਨਕਾਬ[i] = ਝੂਠ;
	}

	ਵਾਪਸ c;
}

ਖਾਲੀ Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::ਸਦਿਸ਼<MemRd> new_rd_ports;
	std::ਸਦਿਸ਼<MemWr> new_wr_ports;
	std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>> new_rd_map;
	std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>> new_wr_map;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ports); i++) {
		ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = rd_ports[i];
		ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << ਪੋਰਟ.wide_log2); sub++) {
			new_rd_map.ਧੱਕੋ_ਪਿੱਠ(std::make_pair(i, sub));
		}
	}
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++) {
		ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = wr_ports[i];
		ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << ਪੋਰਟ.wide_log2); sub++) {
			new_wr_map.ਧੱਕੋ_ਪਿੱਠ(std::make_pair(i, sub));
		}
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : new_rd_map) {
		MemRd &orig = rd_ports[it.ਪਹਿਲਾ];
		MemRd ਪੋਰਟ = orig;
		ਜੇ (it.ਦੂਜਾ != 0)
			ਪੋਰਟ.cell = ਖਾਲੀ_ਸੂਚਕ;
		ਜੇ (ਪੋਰਟ.wide_log2) {
			ਪੋਰਟ.ਅੰਕੜੇ = ਪੋਰਟ.ਅੰਕੜੇ.ਕੱਢੋ_2(it.ਦੂਜਾ * ਚੌੜਾਈ, ਚੌੜਾਈ);
			ਪੋਰਟ.init_ਮੁੱਲ = ਪੋਰਟ.init_ਮੁੱਲ.ਕੱਢੋ_2(it.ਦੂਜਾ * ਚੌੜਾਈ, ਚੌੜਾਈ);
			ਪੋਰਟ.arst_ਮੁੱਲ = ਪੋਰਟ.arst_ਮੁੱਲ.ਕੱਢੋ_2(it.ਦੂਜਾ * ਚੌੜਾਈ, ਚੌੜਾਈ);
			ਪੋਰਟ.srst_ਮੁੱਲ = ਪੋਰਟ.srst_ਮੁੱਲ.ਕੱਢੋ_2(it.ਦੂਜਾ * ਚੌੜਾਈ, ਚੌੜਾਈ);
			ਪੋਰਟ.addr = ਪੋਰਟ.sub_addr(it.ਦੂਜਾ);
			ਪੋਰਟ.wide_log2 = 0;
		}
		ਪੋਰਟ.transparency_ਨਕਾਬ.clear();
		ਪੋਰਟ.collision_x_ਨਕਾਬ.clear();
		ਲਈ (ਸਵੈਚਾਲਿਤ &it2 : new_wr_map)
			ਪੋਰਟ.transparency_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(orig.transparency_ਨਕਾਬ[it2.ਪਹਿਲਾ]);
		ਲਈ (ਸਵੈਚਾਲਿਤ &it2 : new_wr_map)
			ਪੋਰਟ.collision_x_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(orig.collision_x_ਨਕਾਬ[it2.ਪਹਿਲਾ]);
		new_rd_ports.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : new_wr_map) {
		MemWr &orig = wr_ports[it.ਪਹਿਲਾ];
		MemWr ਪੋਰਟ = orig;
		ਜੇ (it.ਦੂਜਾ != 0)
			ਪੋਰਟ.cell = ਖਾਲੀ_ਸੂਚਕ;
		ਜੇ (ਪੋਰਟ.wide_log2) {
			ਪੋਰਟ.ਅੰਕੜੇ = ਪੋਰਟ.ਅੰਕੜੇ.ਕੱਢੋ_2(it.ਦੂਜਾ * ਚੌੜਾਈ, ਚੌੜਾਈ);
			ਪੋਰਟ.en = ਪੋਰਟ.en.ਕੱਢੋ_2(it.ਦੂਜਾ * ਚੌੜਾਈ, ਚੌੜਾਈ);
			ਪੋਰਟ.addr = ਪੋਰਟ.sub_addr(it.ਦੂਜਾ);
			ਪੋਰਟ.wide_log2 = 0;
		}
		ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ.clear();
		ਲਈ (ਸਵੈਚਾਲਿਤ &it2 : new_wr_map)
			ਪੋਰਟ.ਤਰਜੀਹ_ਨਕਾਬ.ਧੱਕੋ_ਪਿੱਠ(orig.ਤਰਜੀਹ_ਨਕਾਬ[it2.ਪਹਿਲਾ]);
		new_wr_ports.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ);
	}
	std::ਅਦਲਾ_ਬਦਲੀ(rd_ports, new_rd_ports);
	std::ਅਦਲਾ_ਬਦਲੀ(wr_ports, new_wr_ports);
}

ਖਾਲੀ Mem::emulate_ਤਰਜੀਹ(ਪੂਰਨ idx1, ਪੂਰਨ idx2, FfInitVals *initvals)
{
	ਸਵੈਚਾਲਿਤ &port1 = wr_ports[idx1];
	ਸਵੈਚਾਲਿਤ &port2 = wr_ports[idx2];
	ਜੇ (!port2.ਤਰਜੀਹ_ਨਕਾਬ[idx1])
		ਵਾਪਸ;
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ports); i++) {
		ਸਵੈਚਾਲਿਤ &rport = rd_ports[i];
		ਜੇ (rport.removed)
			ਜਾਰੀ;
		ਜੇ (rport.transparency_ਨਕਾਬ[idx1] && !(rport.transparency_ਨਕਾਬ[idx2] || rport.collision_x_ਨਕਾਬ[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	ਪੂਰਨ min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	ਪੂਰਨ max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	ਤਾਰਕਿਕ wide1 = port1.wide_log2 > port2.wide_log2;
	ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		ਜੇ (wide1)
			addr1 = port1.sub_addr(sub);
		ਨਹੀਂਤਾਂ
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		ਪੂਰਨ ewidth = ਚੌੜਾਈ << min_wide_log2;
		ਪੂਰਨ sub1 = wide1 ? sub : 0;
		ਪੂਰਨ sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> cache;
		ਲਈ (ਪੂਰਨ pos = 0; pos < ewidth; pos++) {
			Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &en1 = port1.en[pos + sub1 * ਚੌੜਾਈ];
			Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &en2 = port2.en[pos + sub2 * ਚੌੜਾਈ];
			std::pair<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਕੁੰਜੀ(en1, en2);
			ਜੇ (cache.ਗਿਣਤੀ_2(ਕੁੰਜੀ)) {
				en1 = cache[ਕੁੰਜੀ];
			} ਨਹੀਂਤਾਂ {
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[ਕੁੰਜੀ] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.ਤਰਜੀਹ_ਨਕਾਬ[idx1] = ਝੂਠ;
}

ਖਾਲੀ Mem::emulate_transparency(ਪੂਰਨ widx, ਪੂਰਨ ridx, FfInitVals *initvals) {
	ਸਵੈਚਾਲਿਤ &wport = wr_ports[widx];
	ਸਵੈਚਾਲਿਤ &rport = rd_ports[ridx];
	log_assert(rport.transparency_ਨਕਾਬ[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(wr_ports) - 1; i > widx; i--) {
		ਜੇ (wr_ports[i].removed)
			ਜਾਰੀ;
		ਜੇ (rport.transparency_ਨਕਾਬ[i] && wr_ports[i].ਤਰਜੀਹ_ਨਕਾਬ[widx])
			emulate_transparency(i, ridx, initvals);
	}
	ਪੂਰਨ min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	ਪੂਰਨ max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	ਤਾਰਕਿਕ wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(wport.ਅੰਕੜੇ));
	module->ਜੋੜੋ_Dffe(NEW_ID, rport.clk, rport.en, wport.ਅੰਕੜੇ, wdata_q, rport.clk_polarity, ਸੱਚ);
	ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		ਲਈ (ਪੂਰਨ j = min_wide_log2; j < max_wide_log2; j++)
			ਜੇ (wide_write)
				waddr = wport.sub_addr(sub);
			ਨਹੀਂਤਾਂ
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		ਜੇ (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		ਪੂਰਨ pos = 0;
		ਪੂਰਨ ewidth = ਚੌੜਾਈ << min_wide_log2;
		ਪੂਰਨ wsub = wide_write ? sub : 0;
		ਪੂਰਨ rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->ਜੋੜੋ_Wire(NEW_ID, ewidth);
		ਜਦੋਂ (pos < ewidth) {
			ਪੂਰਨ epos = pos;
			ਜਦੋਂ (epos < ewidth && wport.en[epos + wsub * ਚੌੜਾਈ] == wport.en[pos + wsub * ਚੌੜਾਈ])
				epos++;
			SigSpec cond;
			ਜੇ (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * ਚੌੜਾਈ], addr_eq);
			ਨਹੀਂਤਾਂ
				cond = wport.en[pos + wsub * ਚੌੜਾਈ];
			SigSpec cond_q = module->ਜੋੜੋ_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_ਅੰਕੜੇ ff(module, initvals, NEW_ID);
			ff.ਚੌੜਾਈ = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = ਸੱਚ;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			ਜੇ (rport.en != State::S1) {
				ff.has_ce = ਸੱਚ;
				ff.sig_ce = rport.en;
				ff.pol_ce = ਸੱਚ;
			}
			ਜੇ (rport.arst != State::S0) {
				ff.has_arst = ਸੱਚ;
				ff.sig_arst = rport.arst;
				ff.pol_arst = ਸੱਚ;
				ff.val_arst = State::S0;
			}
			ਜੇ (rport.srst != State::S0) {
				ff.has_srst = ਸੱਚ;
				ff.sig_srst = rport.srst;
				ff.pol_srst = ਸੱਚ;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			ਜੇ (!rport.init_ਮੁੱਲ.is_fully_undef())
				ff.val_init = State::S0;
			ਨਹੀਂਤਾਂ
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.ਕੱਢੋ_2(pos, epos-pos);
			SigSpec other = wdata_q.ਕੱਢੋ_2(pos + wsub * ਚੌੜਾਈ, epos-pos);
			SigSpec dest = rport.ਅੰਕੜੇ.ਕੱਢੋ_2(pos + rsub * ਚੌੜਾਈ, epos-pos);
			module->ਜੋੜੋ_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.ਅੰਕੜੇ.replace(rsub * ਚੌੜਾਈ, rdata_a);
	}
	rport.transparency_ਨਕਾਬ[widx] = ਝੂਠ;
	rport.collision_x_ਨਕਾਬ[widx] = ਸੱਚ;
}

ਖਾਲੀ Mem::prepare_wr_ਮਿਲਾਓ(ਪੂਰਨ idx1, ਪੂਰਨ idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	ਸਵੈਚਾਲਿਤ &port1 = wr_ports[idx1];
	ਸਵੈਚਾਲਿਤ &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	ਲਈ (ਪੂਰਨ i = 0; i < idx1; i++)
		ਜੇ (port2.ਤਰਜੀਹ_ਨਕਾਬ[i])
			port1.ਤਰਜੀਹ_ਨਕਾਬ[i] = ਸੱਚ;
	// If port 2 has priority over a port after port 1, emulate it.
	ਲਈ (ਪੂਰਨ i = idx1 + 1; i < idx2; i++)
		ਜੇ (port2.ਤਰਜੀਹ_ਨਕਾਬ[i] && !wr_ports[i].removed)
			emulate_ਤਰਜੀਹ(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	ਲਈ (ਪੂਰਨ i = idx2 + 1; i < Get_ਮਾਪ(wr_ports); i++) {
		ਸਵੈਚਾਲਿਤ &oport = wr_ports[i];
		ਜੇ (oport.ਤਰਜੀਹ_ਨਕਾਬ[idx2])
			oport.ਤਰਜੀਹ_ਨਕਾਬ[idx1] = ਸੱਚ;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ports); i++) {
		ਸਵੈਚਾਲਿਤ &rport = rd_ports[i];
		ਜੇ (rport.removed)
			ਜਾਰੀ;
		// If collision already undefined with both ports, it's fine.
		ਜੇ (rport.collision_x_ਨਕਾਬ[idx1] && rport.collision_x_ਨਕਾਬ[idx2])
			ਜਾਰੀ;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		ਜੇ (rport.collision_x_ਨਕਾਬ[idx1]) {
			rport.collision_x_ਨਕਾਬ[idx1] = ਝੂਠ;
			rport.transparency_ਨਕਾਬ[idx1] = rport.transparency_ਨਕਾਬ[idx2];
			ਜਾਰੀ;
		}
		ਜੇ (rport.collision_x_ਨਕਾਬ[idx2]) {
			rport.collision_x_ਨਕਾਬ[idx2] = ਝੂਠ;
			rport.transparency_ਨਕਾਬ[idx2] = rport.transparency_ਨਕਾਬ[idx1];
			ਜਾਰੀ;
		}
		// If transparent with both ports, also fine.
		ਜੇ (rport.transparency_ਨਕਾਬ[idx1] && rport.transparency_ਨਕਾਬ[idx2])
			ਜਾਰੀ;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		ਜੇ (rport.transparency_ਨਕਾਬ[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_ਨਕਾਬ[idx1] = ਝੂਠ;
			ਜਾਰੀ;
		}
		ਜੇ (rport.transparency_ਨਕਾਬ[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_ਨਕਾਬ[idx2] = ਝੂਠ;
			ਜਾਰੀ;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

ਖਾਲੀ Mem::prepare_rd_ਮਿਲਾਓ(ਪੂਰਨ idx1, ਪੂਰਨ idx2, FfInitVals *initvals) {
	ਸਵੈਚਾਲਿਤ &port1 = rd_ports[idx1];
	ਸਵੈਚਾਲਿਤ &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wr_ports); i++) {
		ਜੇ (wr_ports[i].removed)
			ਜਾਰੀ;
		// Both ports undefined, OK.
		ਜੇ (port1.collision_x_ਨਕਾਬ[i] && port2.collision_x_ਨਕਾਬ[i])
			ਜਾਰੀ;
		// Only one port undefined — change its behavior
		// to align with the other port.
		ਜੇ (port1.collision_x_ਨਕਾਬ[i]) {
			port1.collision_x_ਨਕਾਬ[i] = ਝੂਠ;
			port1.transparency_ਨਕਾਬ[i] = port2.transparency_ਨਕਾਬ[i];
			ਜਾਰੀ;
		}
		ਜੇ (port2.collision_x_ਨਕਾਬ[i]) {
			port2.collision_x_ਨਕਾਬ[i] = ਝੂਠ;
			port2.transparency_ਨਕਾਬ[i] = port1.transparency_ਨਕਾਬ[i];
			ਜਾਰੀ;
		}
		// Both ports transparent, OK.
		ਜੇ (port1.transparency_ਨਕਾਬ[i] && port2.transparency_ਨਕਾਬ[i])
			ਜਾਰੀ;
		// Only one port transparent — emulate transparency
		// on the other.
		ਜੇ (port1.transparency_ਨਕਾਬ[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_ਨਕਾਬ[i] = ਝੂਠ;
			ਜਾਰੀ;
		}
		ਜੇ (port2.transparency_ਨਕਾਬ[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_ਨਕਾਬ[i] = ਝੂਠ;
			ਜਾਰੀ;
		}
		// No ports transparent, OK.
	}

}

ਖਾਲੀ Mem::widen_prep(ਪੂਰਨ wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	ਪੂਰਨ ਨਕਾਬ = ((1 << wide_log2) - 1);
	ਪੂਰਨ delta = ਸ਼ੁਰੂ_ਵਿੱਥ & ਨਕਾਬ;
	ਸ਼ੁਰੂ_ਵਿੱਥ -= delta;
	ਮਾਪ += delta;
	ਜੇ (ਮਾਪ & ਨਕਾਬ) {
		ਮਾਪ |= ਨਕਾਬ;
		ਮਾਪ++;
	}
}

ਖਾਲੀ Mem::widen_wr_ਪੋਰਟ(ਪੂਰਨ idx, ਪੂਰਨ wide_log2) {
	widen_prep(wide_log2);
	ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = wr_ports[idx];
	log_assert(ਪੋਰਟ.wide_log2 <= wide_log2);
	ਜੇ (ਪੋਰਟ.wide_log2 < wide_log2) {
		SigSpec new_ਅੰਕੜੇ, new_en;
		SigSpec addr_lo = ਪੋਰਟ.addr.ਕੱਢੋ_2(0, wide_log2);
		ਲਈ (ਪੂਰਨ sub = 0; sub < (1 << wide_log2); sub += (1 << ਪੋਰਟ.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			ਜੇ (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_ਅੰਕੜੇ.append(ਪੋਰਟ.ਅੰਕੜੇ);
				new_en.append(ਪੋਰਟ.en);
			} ਨਹੀਂਤਾਂ ਜੇ (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_ਅੰਕੜੇ.append(Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ)));
				new_en.append(Const(State::S0, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ)));
			} ਨਹੀਂਤਾਂ {
				// May or may not write to this subword.
				new_ਅੰਕੜੇ.append(ਪੋਰਟ.ਅੰਕੜੇ);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ)), ਪੋਰਟ.en, addr_eq);
				new_en.append(en);
			}
		}
		ਪੋਰਟ.addr.replace(ਪੋਰਟ.wide_log2, Const(State::S0, wide_log2 - ਪੋਰਟ.wide_log2));
		ਪੋਰਟ.ਅੰਕੜੇ = new_ਅੰਕੜੇ;
		ਪੋਰਟ.en = new_en;
		ਪੋਰਟ.wide_log2 = wide_log2;
	}
}

ਖਾਲੀ Mem::emulate_rden(ਪੂਰਨ idx, FfInitVals *initvals) {
	ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = rd_ports[idx];
	log_assert(ਪੋਰਟ.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_ਅੰਕੜੇ = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	Wire *prev_ਅੰਕੜੇ = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	Wire *sel = module->ਜੋੜੋ_Wire(NEW_ID);
	Ff_ਅੰਕੜੇ ff_sel(module, initvals, NEW_ID);
	Ff_ਅੰਕੜੇ ff_ਅੰਕੜੇ(module, initvals, NEW_ID);
	ff_sel.ਚੌੜਾਈ = 1;
	ff_sel.has_clk = ਸੱਚ;
	ff_sel.sig_clk = ਪੋਰਟ.clk;
	ff_sel.pol_clk = ਪੋਰਟ.clk_polarity;
	ff_sel.sig_d = ਪੋਰਟ.en;
	ff_sel.sig_q = sel;
	ff_ਅੰਕੜੇ.ਚੌੜਾਈ = Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ);
	ff_ਅੰਕੜੇ.has_clk = ਸੱਚ;
	ff_ਅੰਕੜੇ.sig_clk = ਪੋਰਟ.clk;
	ff_ਅੰਕੜੇ.pol_clk = ਪੋਰਟ.clk_polarity;
	ff_ਅੰਕੜੇ.sig_d = ਪੋਰਟ.ਅੰਕੜੇ;
	ff_ਅੰਕੜੇ.sig_q = prev_ਅੰਕੜੇ;
	ਜੇ (!ਪੋਰਟ.init_ਮੁੱਲ.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_ਅੰਕੜੇ.val_init = ਪੋਰਟ.init_ਮੁੱਲ;
		ਪੋਰਟ.init_ਮੁੱਲ = Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	} ਨਹੀਂਤਾਂ {
		ff_sel.val_init = State::Sx;
		ff_ਅੰਕੜੇ.val_init = Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	}
	ਜੇ (ਪੋਰਟ.arst != State::S0) {
		ff_sel.has_arst = ਸੱਚ;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = ਪੋਰਟ.arst;
		ff_sel.pol_arst = ਸੱਚ;
		ff_ਅੰਕੜੇ.has_arst = ਸੱਚ;
		ff_ਅੰਕੜੇ.val_arst = ਪੋਰਟ.arst_ਮੁੱਲ;
		ff_ਅੰਕੜੇ.sig_arst = ਪੋਰਟ.arst;
		ff_ਅੰਕੜੇ.pol_arst = ਸੱਚ;
		ਪੋਰਟ.arst = State::S0;
	}
	ਜੇ (ਪੋਰਟ.srst != State::S0) {
		log_assert(!ਪੋਰਟ.ce_over_srst);
		ff_sel.has_srst = ਸੱਚ;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = ਪੋਰਟ.srst;
		ff_sel.pol_srst = ਸੱਚ;
		ff_sel.ce_over_srst = ਝੂਠ;
		ff_ਅੰਕੜੇ.has_srst = ਸੱਚ;
		ff_ਅੰਕੜੇ.val_srst = ਪੋਰਟ.srst_ਮੁੱਲ;
		ff_ਅੰਕੜੇ.sig_srst = ਪੋਰਟ.srst;
		ff_ਅੰਕੜੇ.pol_srst = ਸੱਚ;
		ff_ਅੰਕੜੇ.ce_over_srst = ਝੂਠ;
		ਪੋਰਟ.srst = State::S0;
	}
	ff_sel.emit();
	ff_ਅੰਕੜੇ.emit();
	module->ਜੋੜੋ_Mux(NEW_ID, prev_ਅੰਕੜੇ, new_ਅੰਕੜੇ, sel, ਪੋਰਟ.ਅੰਕੜੇ);
	ਪੋਰਟ.ਅੰਕੜੇ = new_ਅੰਕੜੇ;
	ਪੋਰਟ.en = State::S1;
}

ਖਾਲੀ Mem::emulate_reset(ਪੂਰਨ idx, ਤਾਰਕਿਕ emu_init, ਤਾਰਕਿਕ emu_arst, ਤਾਰਕਿਕ emu_srst, FfInitVals *initvals) {
	ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = rd_ports[idx];
	ਜੇ (emu_init && !ਪੋਰਟ.init_ਮੁੱਲ.is_fully_undef()) {
		Wire *sel = module->ਜੋੜੋ_Wire(NEW_ID);
		Ff_ਅੰਕੜੇ ff_sel(module, initvals, NEW_ID);
		Wire *new_ਅੰਕੜੇ = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
		ff_sel.ਚੌੜਾਈ = 1;
		ff_sel.has_clk = ਸੱਚ;
		ff_sel.sig_clk = ਪੋਰਟ.clk;
		ff_sel.pol_clk = ਪੋਰਟ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		ਜੇ (ਪੋਰਟ.en != State::S1) {
			ff_sel.has_ce = ਸੱਚ;
			ff_sel.sig_ce = ਪੋਰਟ.en;
			ff_sel.pol_ce = ਸੱਚ;
			ff_sel.ce_over_srst = ਪੋਰਟ.ce_over_srst;
		}
		ਜੇ (ਪੋਰਟ.arst != State::S0) {
			ff_sel.has_arst = ਸੱਚ;
			ff_sel.sig_arst = ਪੋਰਟ.arst;
			ff_sel.pol_arst = ਸੱਚ;
			ਜੇ (emu_arst && ਪੋਰਟ.arst_ਮੁੱਲ == ਪੋਰਟ.init_ਮੁੱਲ) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				ਪੋਰਟ.arst = State::S0;
			} ਨਹੀਂਤਾਂ {
				ff_sel.val_arst = State::S1;
			}
		}
		ਜੇ (ਪੋਰਟ.srst != State::S0) {
			ff_sel.has_srst = ਸੱਚ;
			ff_sel.sig_srst = ਪੋਰਟ.srst;
			ff_sel.pol_srst = ਸੱਚ;
			ਜੇ (emu_srst && ਪੋਰਟ.srst_ਮੁੱਲ == ਪੋਰਟ.init_ਮੁੱਲ) {
				ff_sel.val_srst = State::S0;
				ਪੋਰਟ.srst = State::S0;
			} ਨਹੀਂਤਾਂ {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->ਜੋੜੋ_Mux(NEW_ID, ਪੋਰਟ.init_ਮੁੱਲ, new_ਅੰਕੜੇ, sel, ਪੋਰਟ.ਅੰਕੜੇ);
		ਪੋਰਟ.ਅੰਕੜੇ = new_ਅੰਕੜੇ;
		ਪੋਰਟ.init_ਮੁੱਲ = Const(State::Sx, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
	}
	ਜੇ (emu_arst && ਪੋਰਟ.arst != State::S0) {
		Wire *sel = module->ਜੋੜੋ_Wire(NEW_ID);
		Ff_ਅੰਕੜੇ ff_sel(module, initvals, NEW_ID);
		Wire *new_ਅੰਕੜੇ = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
		ff_sel.ਚੌੜਾਈ = 1;
		ff_sel.has_clk = ਸੱਚ;
		ff_sel.sig_clk = ਪੋਰਟ.clk;
		ff_sel.pol_clk = ਪੋਰਟ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ਜੇ (ਪੋਰਟ.init_ਮੁੱਲ.is_fully_undef())
			ff_sel.val_init = State::Sx;
		ਨਹੀਂਤਾਂ
			ff_sel.val_init = State::S1;
		ਜੇ (ਪੋਰਟ.en != State::S1) {
			ff_sel.has_ce = ਸੱਚ;
			ff_sel.sig_ce = ਪੋਰਟ.en;
			ff_sel.pol_ce = ਸੱਚ;
			ff_sel.ce_over_srst = ਪੋਰਟ.ce_over_srst;
		}
		ff_sel.has_arst = ਸੱਚ;
		ff_sel.sig_arst = ਪੋਰਟ.arst;
		ff_sel.pol_arst = ਸੱਚ;
		ff_sel.val_arst = State::S0;
		ਜੇ (ਪੋਰਟ.srst != State::S0) {
			ff_sel.has_srst = ਸੱਚ;
			ff_sel.sig_srst = ਪੋਰਟ.srst;
			ff_sel.pol_srst = ਸੱਚ;
			ਜੇ (emu_srst && ਪੋਰਟ.srst_ਮੁੱਲ == ਪੋਰਟ.arst_ਮੁੱਲ) {
				ff_sel.val_srst = State::S0;
				ਪੋਰਟ.srst = State::S0;
			} ਨਹੀਂਤਾਂ {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->ਜੋੜੋ_Mux(NEW_ID, ਪੋਰਟ.arst_ਮੁੱਲ, new_ਅੰਕੜੇ, sel, ਪੋਰਟ.ਅੰਕੜੇ);
		ਪੋਰਟ.ਅੰਕੜੇ = new_ਅੰਕੜੇ;
		ਪੋਰਟ.arst = State::S0;
	}
	ਜੇ (emu_srst && ਪੋਰਟ.srst != State::S0) {
		Wire *sel = module->ਜੋੜੋ_Wire(NEW_ID);
		Ff_ਅੰਕੜੇ ff_sel(module, initvals, NEW_ID);
		Wire *new_ਅੰਕੜੇ = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
		ff_sel.ਚੌੜਾਈ = 1;
		ff_sel.has_clk = ਸੱਚ;
		ff_sel.sig_clk = ਪੋਰਟ.clk;
		ff_sel.pol_clk = ਪੋਰਟ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ਜੇ (ਪੋਰਟ.init_ਮੁੱਲ.is_fully_undef())
			ff_sel.val_init = State::Sx;
		ਨਹੀਂਤਾਂ
			ff_sel.val_init = State::S1;
		ਜੇ (ਪੋਰਟ.en != State::S1) {
			ff_sel.has_ce = ਸੱਚ;
			ff_sel.sig_ce = ਪੋਰਟ.en;
			ff_sel.pol_ce = ਸੱਚ;
			ff_sel.ce_over_srst = ਪੋਰਟ.ce_over_srst;
		}
		ff_sel.has_srst = ਸੱਚ;
		ff_sel.sig_srst = ਪੋਰਟ.srst;
		ff_sel.pol_srst = ਸੱਚ;
		ff_sel.val_srst = State::S0;
		ਜੇ (ਪੋਰਟ.arst != State::S0) {
			ff_sel.has_arst = ਸੱਚ;
			ff_sel.sig_arst = ਪੋਰਟ.arst;
			ff_sel.pol_arst = ਸੱਚ;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->ਜੋੜੋ_Mux(NEW_ID, ਪੋਰਟ.srst_ਮੁੱਲ, new_ਅੰਕੜੇ, sel, ਪੋਰਟ.ਅੰਕੜੇ);
		ਪੋਰਟ.ਅੰਕੜੇ = new_ਅੰਕੜੇ;
		ਪੋਰਟ.srst = State::S0;
	}
}

ਖਾਲੀ Mem::emulate_rd_ce_over_srst(ਪੂਰਨ idx) {
	ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = rd_ports[idx];
	log_assert(ਪੋਰਟ.clk_enable);
	ਜੇ (ਪੋਰਟ.en == State::S1 || ਪੋਰਟ.srst == State::S0 || !ਪੋਰਟ.ce_over_srst) {
		ਪੋਰਟ.ce_over_srst = ਝੂਠ;
		ਵਾਪਸ;
	}
	ਪੋਰਟ.ce_over_srst = ਝੂਠ;
	ਪੋਰਟ.srst = module->And(NEW_ID, ਪੋਰਟ.en, ਪੋਰਟ.srst);
}

ਖਾਲੀ Mem::emulate_rd_srst_over_ce(ਪੂਰਨ idx) {
	ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = rd_ports[idx];
	log_assert(ਪੋਰਟ.clk_enable);
	ਜੇ (ਪੋਰਟ.en == State::S1 || ਪੋਰਟ.srst == State::S0 || ਪੋਰਟ.ce_over_srst) {
		ਪੋਰਟ.ce_over_srst = ਸੱਚ;
		ਵਾਪਸ;
	}
	ਪੋਰਟ.ce_over_srst = ਸੱਚ;
	ਪੋਰਟ.en = module->Or(NEW_ID, ਪੋਰਟ.en, ਪੋਰਟ.srst);
}

ਤਾਰਕਿਕ Mem::emulate_read_ਪਹਿਲਾ_ok() {
	ਜੇ (wr_ports.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ ਝੂਠ;
	SigSpec clk = wr_ports[0].clk;
	ਤਾਰਕਿਕ clk_polarity = wr_ports[0].clk_polarity;
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ: wr_ports) {
		ਜੇ (!ਪੋਰਟ.clk_enable)
			ਵਾਪਸ ਝੂਠ;
		ਜੇ (ਪੋਰਟ.clk != clk)
			ਵਾਪਸ ਝੂਠ;
		ਜੇ (ਪੋਰਟ.clk_polarity != clk_polarity)
			ਵਾਪਸ ਝੂਠ;
	}
	ਤਾਰਕਿਕ ਮਿਲਿਆ_read_ਪਹਿਲਾ = ਝੂਠ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ: rd_ports) {
		ਜੇ (!ਪੋਰਟ.clk_enable)
			ਵਾਪਸ ਝੂਠ;
		ਜੇ (ਪੋਰਟ.clk != clk)
			ਵਾਪਸ ਝੂਠ;
		ਜੇ (ਪੋਰਟ.clk_polarity != clk_polarity)
			ਵਾਪਸ ਝੂਠ;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(wr_ports); j++)
			ਜੇ (!ਪੋਰਟ.transparency_ਨਕਾਬ[j] && !ਪੋਰਟ.collision_x_ਨਕਾਬ[j])
				ਮਿਲਿਆ_read_ਪਹਿਲਾ = ਸੱਚ;
	}
	ਵਾਪਸ ਮਿਲਿਆ_read_ਪਹਿਲਾ;
}

ਖਾਲੀ Mem::emulate_read_ਪਹਿਲਾ(FfInitVals *initvals) {
	log_assert(emulate_read_ਪਹਿਲਾ_ok());
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ports); i++)
		ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(wr_ports); j++)
			ਜੇ (rd_ports[i].transparency_ਨਕਾਬ[j])
				emulate_transparency(j, i, initvals);
	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(rd_ports); i++)
		ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_ਨਕਾਬ[j]);
			rd_ports[i].collision_x_ਨਕਾਬ[j] = ਝੂਠ;
			rd_ports[i].transparency_ਨਕਾਬ[j] = ਸੱਚ;
		}
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ: wr_ports) {
		Wire *new_ਅੰਕੜੇ = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ));
		Wire *new_addr = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(ਪੋਰਟ.addr));
		ਸਵੈਚਾਲਿਤ compressed = ਪੋਰਟ.compress_en();
		Wire *new_en = module->ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(compressed.ਪਹਿਲਾ));
		Ff_ਅੰਕੜੇ ff_ਅੰਕੜੇ(module, initvals, NEW_ID);
		Ff_ਅੰਕੜੇ ff_addr(module, initvals, NEW_ID);
		Ff_ਅੰਕੜੇ ff_en(module, initvals, NEW_ID);
		ff_ਅੰਕੜੇ.ਚੌੜਾਈ = Get_ਮਾਪ(ਪੋਰਟ.ਅੰਕੜੇ);
		ff_ਅੰਕੜੇ.has_clk = ਸੱਚ;
		ff_ਅੰਕੜੇ.sig_clk = ਪੋਰਟ.clk;
		ff_ਅੰਕੜੇ.pol_clk = ਪੋਰਟ.clk_polarity;
		ff_ਅੰਕੜੇ.sig_d = ਪੋਰਟ.ਅੰਕੜੇ;
		ff_ਅੰਕੜੇ.sig_q = new_ਅੰਕੜੇ;;
		ff_ਅੰਕੜੇ.val_init = Const(State::Sx, ff_ਅੰਕੜੇ.ਚੌੜਾਈ);
		ff_ਅੰਕੜੇ.emit();
		ff_addr.ਚੌੜਾਈ = Get_ਮਾਪ(ਪੋਰਟ.addr);
		ff_addr.has_clk = ਸੱਚ;
		ff_addr.sig_clk = ਪੋਰਟ.clk;
		ff_addr.pol_clk = ਪੋਰਟ.clk_polarity;
		ff_addr.sig_d = ਪੋਰਟ.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.ਚੌੜਾਈ);
		ff_addr.emit();
		ff_en.ਚੌੜਾਈ = Get_ਮਾਪ(compressed.ਪਹਿਲਾ);
		ff_en.has_clk = ਸੱਚ;
		ff_en.sig_clk = ਪੋਰਟ.clk;
		ff_en.pol_clk = ਪੋਰਟ.clk_polarity;
		ff_en.sig_d = compressed.ਪਹਿਲਾ;
		ff_en.sig_q = new_en;;
		ਜੇ (inits.ਖਾਲੀ_ਥਾਵਾਂ())
			ff_en.val_init = Const(State::Sx, ff_en.ਚੌੜਾਈ);
		ਨਹੀਂਤਾਂ
			ff_en.val_init = Const(State::S0, ff_en.ਚੌੜਾਈ);
		ff_en.emit();
		ਪੋਰਟ.ਅੰਕੜੇ = new_ਅੰਕੜੇ;
		ਪੋਰਟ.addr = new_addr;
		ਪੋਰਟ.en = ਪੋਰਟ.decompress_en(compressed.ਦੂਜਾ, new_en);
	}
}

std::pair<SigSpec, std::ਸਦਿਸ਼<ਪੂਰਨ>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::ਸਦਿਸ਼<ਪੂਰਨ> swizzle;
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ prev_ਦੋਆਧਾਰੀ_ਅੰਕ = en[0];
	ਪੂਰਨ idx = 0;
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ: en) {
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != prev_ਦੋਆਧਾਰੀ_ਅੰਕ) {
			sig.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
			prev_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ;
			idx++;
		}
		swizzle.ਧੱਕੋ_ਪਿੱਠ(idx);
	}
	log_assert(idx + 1 == Get_ਮਾਪ(sig));
	ਵਾਪਸ {sig, swizzle};
}

SigSpec MemWr::decompress_en(ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &swizzle, SigSpec sig) {
	SigSpec res;
	ਲਈ (ਪੂਰਨ i: swizzle)
		res.append(sig[i]);
	ਵਾਪਸ res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->ਮਾਪ), mem->ਚੌੜਾਈ)
{
	ਲਈ(ਅਚਲ ਸਵੈਚਾਲਿਤ &init : mem->inits) {
		ਜੇ(init.en.is_fully_zero()) ਜਾਰੀ;
		log_assert(init.en.ਮਾਪ() == ਅੰਕੜੇ_ਚੌੜਾਈ_2);
		ਜੇ(init.en.is_fully_ones())
			ਪਾਓ_concatenated(init.addr.as_int(), init.ਅੰਕੜੇ);
		ਨਹੀਂਤਾਂ {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t ਸ਼ਬਦ_3 = init.ਅੰਕੜੇ.ਮਾਪ() / ਅੰਕੜੇ_ਚੌੜਾਈ_2;
			RTLIL::Const ਅੰਕੜੇ = init.ਅੰਕੜੇ;
			log_assert(ਅੰਕੜੇ.ਮਾਪ() % ਅੰਕੜੇ_ਚੌੜਾਈ_2 == 0);
			ਲਈ(addr_t i = 0; i < ਸ਼ਬਦ_3; i++) {
				RTLIL::Const ਪਿਛਲਾ = (*ਇਹ)[addr + i];
				ਲਈ(ਪੂਰਨ j = 0; j < ਅੰਕੜੇ_ਚੌੜਾਈ_2; j++)
					ਜੇ(init.en[j] != State::S1)
						ਅੰਕੜੇ.ਸੈੱਟ(ਅੰਕੜੇ_ਚੌੜਾਈ_2 * i + j, ਪਿਛਲਾ[j]);
			}
			ਪਾਓ_concatenated(init.addr.as_int(), ਅੰਕੜੇ);
		}
	}
}

MemContents::iterator & MemContents::iterator::ਸੰਚਾਲਕ++() {
	ਸਵੈਚਾਲਿਤ it = ਸਮ੍ਰਿਤੀ_2->ਮੁੱਲਾਂ_2.ਉੱਤਲਾ_bound(_addr);
	ਜੇ(it == ਸਮ੍ਰਿਤੀ_2->ਮੁੱਲਾਂ_2.ਸਿਰਾ()) {
		ਸਮ੍ਰਿਤੀ_2 = ਖਾਲੀ_ਸੂਚਕ;
		_addr = ~(addr_t) 0;
	} ਨਹੀਂਤਾਂ
		_addr = it->ਪਹਿਲਾ;
	ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ MemContents::check() {
	log_assert(addr_ਚੌੜਾਈ_2 > 0 && addr_ਚੌੜਾਈ_2 < (ਪੂਰਨ)ਆਕਾਰ(addr_t) * 8);
	log_assert(ਅੰਕੜੇ_ਚੌੜਾਈ_2 > 0);
	log_assert(default_ਮੁੱਲ.ਮਾਪ() == ਅੰਕੜੇ_ਚੌੜਾਈ_2);
	ਜੇ(ਮੁੱਲਾਂ_2.ਖਾਲੀ_ਥਾਵਾਂ()) ਵਾਪਸ;
	ਸਵੈਚਾਲਿਤ it = ਮੁੱਲਾਂ_2.begin();
	ਲਈ(;;) {
		log_assert(!it->ਦੂਜਾ.ਖਾਲੀ_ਥਾਵਾਂ());
		log_assert(it->ਦੂਜਾ.ਮਾਪ() % ਅੰਕੜੇ_ਚੌੜਾਈ_2 == 0);
		ਸਵੈਚਾਲਿਤ end1 = range_ਸਿਰਾ(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_ਚੌੜਾਈ_2));
		log_assert(end1 <= (addr_t)(1<<addr_ਚੌੜਾਈ_2));
		ਜੇ(++it == ਮੁੱਲਾਂ_2.ਸਿਰਾ())
			ਰੋਕੋ;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

ਤਾਰਕਿਕ MemContents::range_ਸ਼ਾਮਲ_ਹੈ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ਅਚਲ {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	ਵਾਪਸ it != ਮੁੱਲਾਂ_2.ਸਿਰਾ() && addr - _range_begin(it) < range_ਮਾਪ(it);
}


ਤਾਰਕਿਕ MemContents::range_ਸ਼ਾਮਲ_ਹੈ(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ਸਿਰਾ_addr) ਅਚਲ {
	// note that we assume begin_addr <= end_addr
	ਵਾਪਸ it != ਮੁੱਲਾਂ_2.ਸਿਰਾ() && _range_begin(it) <= begin_addr && ਸਿਰਾ_addr - _range_begin(it) <= range_ਮਾਪ(it);
}

ਤਾਰਕਿਕ MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t ਸਿਰਾ_addr) ਅਚਲ {
	ਜੇ(it == ਮੁੱਲਾਂ_2.ਸਿਰਾ() || begin_addr >= ਸਿਰਾ_addr)
		ਵਾਪਸ ਝੂਠ;
	ਸਵੈਚਾਲਿਤ top1 = range_ਸਿਰਾ(it) - 1;
	ਸਵੈਚਾਲਿਤ top2 = ਸਿਰਾ_addr - 1;
	ਵਾਪਸ !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ਅਚਲ {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_ਚੌੜਾਈ_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	ਸਵੈਚਾਲਿਤ it = const_cast<std::map<addr_t, RTLIL::Const> &>(ਮੁੱਲਾਂ_2).ਉੱਤਲਾ_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	ਜੇ(it == ਮੁੱਲਾਂ_2.begin())
		ਵਾਪਸ it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	ਸਵੈਚਾਲਿਤ it_prev = std::ਅਗਲਾ(it, -1);
	ਜੇ(range_ਸ਼ਾਮਲ_ਹੈ(it_prev, addr))
		ਵਾਪਸ it_prev;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ it;
}

RTLIL::Const MemContents::ਸੰਚਾਲਕ[](addr_t addr) ਅਚਲ {
	ਸਵੈਚਾਲਿਤ it = _range_at(addr);
	ਜੇ(range_ਸ਼ਾਮਲ_ਹੈ(it, addr))
		ਵਾਪਸ it->ਦੂਜਾ.ਕੱਢੋ_2(range_ਵਿੱਥ(it, addr), ਅੰਕੜੇ_ਚੌੜਾਈ_2);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ default_ਮੁੱਲ;
}

MemContents::addr_t MemContents::ਗਿਣਤੀ_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr) ਅਚਲ {
	addr_t ਗਿਣਤੀ_2 = 0;
	ਲਈ(ਸਵੈਚਾਲਿਤ it = _range_at(begin_addr); _range_overlaps(it, begin_addr, ਸਿਰਾ_addr); it++) {
		ਸਵੈਚਾਲਿਤ ਪਹਿਲਾ = std::max(_range_begin(it), begin_addr);
		ਸਵੈਚਾਲਿਤ ਆਖਰੀ = std::min(range_ਸਿਰਾ(it), ਸਿਰਾ_addr);
		ਗਿਣਤੀ_2 += ਆਖਰੀ - ਪਹਿਲਾ;
	}
	ਵਾਪਸ ਗਿਣਤੀ_2;
}

ਖਾਲੀ MemContents::clear_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr) {
	ਜੇ(begin_addr >= ਸਿਰਾ_addr) ਵਾਪਸ;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	ਸਵੈਚਾਲਿਤ begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	ਸਵੈਚਾਲਿਤ ਸਿਰਾ_it = ਮੁੱਲਾਂ_2.ਉੱਤਲਾ_bound(ਸਿਰਾ_addr - 1);
	ਜੇ(begin_it == ਸਿਰਾ_it)
		ਵਾਪਸ; // nothing to do
	// the last iterator affected is one before the first one not affected
	ਸਵੈਚਾਲਿਤ ਆਖਰੀ_it = std::ਅਗਲਾ(ਸਿਰਾ_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	ਜੇ(range_ਸ਼ਾਮਲ_ਹੈ(ਆਖਰੀ_it, ਸਿਰਾ_addr - 1)) {
		ਸਵੈਚਾਲਿਤ new_begin = ਸਿਰਾ_addr;
		ਸਵੈਚਾਲਿਤ ਸਿਰਾ = range_ਸਿਰਾ(ਆਖਰੀ_it);
		// if there is data past the end address, preserve it by creating a new range
		ਜੇ(new_begin != ਸਿਰਾ)
			ਸਿਰਾ_it = ਮੁੱਲਾਂ_2.emplace_hint(ਆਖਰੀ_it, new_begin, ਆਖਰੀ_it->ਦੂਜਾ.ਕੱਢੋ_2(range_ਵਿੱਥ(ਆਖਰੀ_it, new_begin), (range_ਸਿਰਾ(ਆਖਰੀ_it) - new_begin) * ਅੰਕੜੇ_ਚੌੜਾਈ_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	ਜੇ(range_ਸ਼ਾਮਲ_ਹੈ(begin_it, begin_addr)) {
		ਸਵੈਚਾਲਿਤ new_ਸਿਰਾ = begin_addr;
		// if there is data before the start address, truncate but don't delete
		ਜੇ(new_ਸਿਰਾ != begin_it->ਪਹਿਲਾ) {
			begin_it->ਦੂਜਾ.extu(range_ਵਿੱਥ(begin_it, new_ਸਿਰਾ));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	ਮੁੱਲਾਂ_2.erase(begin_it, ਸਿਰਾ_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t ਸਿਰਾ_addr) {
	ਜੇ(begin_addr >= ਸਿਰਾ_addr)
		ਵਾਪਸ ਮੁੱਲਾਂ_2.ਸਿਰਾ(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	ਸਵੈਚਾਲਿਤ ਹੇਠਲਾ_it = begin_addr == 0 ? ਮੁੱਲਾਂ_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	ਜੇ (range_ਸ਼ਾਮਲ_ਹੈ(ਹੇਠਲਾ_it, begin_addr, ਸਿਰਾ_addr))
		ਵਾਪਸ ਹੇਠਲਾ_it;
	// find the first range containing any addr >= end_addr
	ਸਵੈਚਾਲਿਤ ਉੱਤਲਾ_it = _range_at(ਸਿਰਾ_addr);
	// check if either of the two ranges we just found touch our range
	ਤਾਰਕਿਕ ਹੇਠਲਾ_touch = begin_addr > 0 && range_ਸ਼ਾਮਲ_ਹੈ(ਹੇਠਲਾ_it, begin_addr - 1);
	ਤਾਰਕਿਕ ਉੱਤਲਾ_touch = range_ਸ਼ਾਮਲ_ਹੈ(ਉੱਤਲਾ_it, ਸਿਰਾ_addr);
	ਜੇ (ਹੇਠਲਾ_touch && ਉੱਤਲਾ_touch) {
		log_assert (ਹੇਠਲਾ_it != ਉੱਤਲਾ_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		ਸਵੈਚਾਲਿਤ ਉੱਤਲਾ_ਸਿਰਾ = range_ਸਿਰਾ(ਉੱਤਲਾ_it);
		// make range bigger (maybe reserve here instead of resize?)
		ਹੇਠਲਾ_it->ਦੂਜਾ.resize(range_ਵਿੱਥ(ਹੇਠਲਾ_it, ਉੱਤਲਾ_ਸਿਰਾ), State::Sx);
		// copy only the data beyond our range
		std::copy(range_ਅੰਕੜੇ(ਉੱਤਲਾ_it, ਸਿਰਾ_addr), range_ਅੰਕੜੇ(ਉੱਤਲਾ_it, ਉੱਤਲਾ_ਸਿਰਾ), range_ਅੰਕੜੇ(ਹੇਠਲਾ_it, ਸਿਰਾ_addr));
		// keep lower_it, but delete upper_it
		ਮੁੱਲਾਂ_2.erase(std::ਅਗਲਾ(ਹੇਠਲਾ_it), std::ਅਗਲਾ(ਉੱਤਲਾ_it));
		ਵਾਪਸ ਹੇਠਲਾ_it;
	} ਨਹੀਂਤਾਂ ਜੇ (ਹੇਠਲਾ_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		ਹੇਠਲਾ_it->ਦੂਜਾ.resize(range_ਵਿੱਥ(ਹੇਠਲਾ_it, ਸਿਰਾ_addr), State::Sx);
		// keep lower_it and upper_it
		ਮੁੱਲਾਂ_2.erase(std::ਅਗਲਾ(ਹੇਠਲਾ_it), ਉੱਤਲਾ_it);
		ਵਾਪਸ ਹੇਠਲਾ_it;
	} ਨਹੀਂਤਾਂ ਜੇ (ਉੱਤਲਾ_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const ਅੰਕੜੇ(State::Sx, (_range_begin(ਉੱਤਲਾ_it) - begin_addr) * ਅੰਕੜੇ_ਚੌੜਾਈ_2);
		ਅੰਕੜੇ.append(std::move(ਉੱਤਲਾ_it->ਦੂਜਾ));
		// delete lower_it and upper_it, then reinsert
		ਮੁੱਲਾਂ_2.erase(ਹੇਠਲਾ_it, std::ਅਗਲਾ(ਉੱਤਲਾ_it));
		ਵਾਪਸ ਮੁੱਲਾਂ_2.emplace(begin_addr, std::move(ਅੰਕੜੇ)).ਪਹਿਲਾ;
	} ਨਹੀਂਤਾਂ {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		ਮੁੱਲਾਂ_2.erase(ਹੇਠਲਾ_it, ਉੱਤਲਾ_it);
		ਵਾਪਸ ਮੁੱਲਾਂ_2.emplace(begin_addr, RTLIL::Const(State::Sx, (ਸਿਰਾ_addr - begin_addr) * ਅੰਕੜੇ_ਚੌੜਾਈ_2)).ਪਹਿਲਾ;
	}
}

ਖਾਲੀ MemContents::ਪਾਓ_concatenated(addr_t addr, RTLIL::Const ਅਚਲ &ਮੁੱਲਾਂ) {
	addr_t ਸ਼ਬਦ_3 = (ਮੁੱਲਾਂ.ਮਾਪ() + ਅੰਕੜੇ_ਚੌੜਾਈ_2 - 1) / ਅੰਕੜੇ_ਚੌੜਾਈ_2;
	log_assert(addr < (addr_t)(1<<addr_ਚੌੜਾਈ_2));
	log_assert(ਸ਼ਬਦ_3 <= (addr_t)(1<<addr_ਚੌੜਾਈ_2) - addr);
	ਸਵੈਚਾਲਿਤ it = _reserve_range(addr, addr + ਸ਼ਬਦ_3);
	ਸਵੈਚਾਲਿਤ ਮੰਜ਼ਿਲ_begin = range_ਅੰਕੜੇ(it, addr);
	std::copy(ਮੁੱਲਾਂ.begin(), ਮੁੱਲਾਂ.ਸਿਰਾ(), ਮੰਜ਼ਿਲ_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(ਮੰਜ਼ਿਲ_begin + ਮੁੱਲਾਂ.ਮਾਪ(), ਮੰਜ਼ਿਲ_begin + ਸ਼ਬਦ_3 * ਅੰਕੜੇ_ਚੌੜਾਈ_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ਅਚਲ &ਸ਼ਬਦ) {
	ਸਵੈਚਾਲਿਤ from_ਸਿਰਾ = ਸ਼ਬਦ.ਮਾਪ() <= ਅੰਕੜੇ_ਚੌੜਾਈ_2 ? ਸ਼ਬਦ.ਸਿਰਾ() : ਸ਼ਬਦ.begin() + ਅੰਕੜੇ_ਚੌੜਾਈ_2;
	ਸਵੈਚਾਲਿਤ ਮੰਜ਼ਿਲ_ਸਿਰਾ = std::copy(ਸ਼ਬਦ.begin(), from_ਸਿਰਾ, it);
	ਸਵੈਚਾਲਿਤ it_ਅਗਲਾ = std::ਅਗਲਾ(it, ਅੰਕੜੇ_ਚੌੜਾਈ_2);
	std::fill(ਮੰਜ਼ਿਲ_ਸਿਰਾ, it_ਅਗਲਾ, State::S0);
	ਵਾਪਸ it_ਅਗਲਾ;
}
