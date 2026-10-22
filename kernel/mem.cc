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

#ضمن "kernel/mem.h"
#ضمن "kernel/ff.h"

USING_YOSYS_NAMESPACE

فراغ Mem::remove() {
	إذا (cell) {
		module->remove(cell);
		cell = تش_اشاره;
	}
	إذا (mem) {
		module->memories.erase(mem->name);
		ړنګول mem;
		mem = تش_اشاره;
	}
	لكل (خپلکاری &پورټ : rd_ports) {
		إذا (پورټ.cell) {
			module->remove(پورټ.cell);
			پورټ.cell = تش_اشاره;
		}
	}
	لكل (خپلکاری &پورټ : wr_ports) {
		إذا (پورټ.cell) {
			module->remove(پورټ.cell);
			پورټ.cell = تش_اشاره;
		}
	}
	لكل (خپلکاری &init : inits) {
		إذا (init.cell) {
			module->remove(init.cell);
			init.cell = تش_اشاره;
		}
	}
}

فراغ Mem::emit() {
	check();
	std::ویکټور<صحيح> rd_کیڼ;
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خپلکاری &پورټ = rd_ports[i];
		إذا (پورټ.removed) {
			إذا (پورټ.cell) {
				module->remove(پورټ.cell);
			}
		} وإلا {
			rd_کیڼ.ټېل_شا(i);
		}
	}
	std::ویکټور<صحيح> wr_کیڼ;
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		خپلکاری &پورټ = wr_ports[i];
		إذا (پورټ.removed) {
			إذا (پورټ.cell) {
				module->remove(پورټ.cell);
			}
		} وإلا {
			wr_کیڼ.ټېل_شا(i);
		}
	}
	std::ویکټور<صحيح> init_کیڼ;
	لكل (صحيح i = 0; i < Get_اندازه(inits); i++) {
		خپلکاری &init = inits[i];
		إذا (init.removed) {
			إذا (init.cell) {
				module->remove(init.cell);
			}
		} وإلا {
			init_کیڼ.ټېل_شا(i);
		}
	}
	لكل (صحيح i = 0; i < Get_اندازه(rd_کیڼ); i++)
		إذا (i != rd_کیڼ[i])
			std::بدلون(rd_ports[i], rd_ports[rd_کیڼ[i]]);
	rd_ports.resize(Get_اندازه(rd_کیڼ));
	لكل (صحيح i = 0; i < Get_اندازه(wr_کیڼ); i++)
		إذا (i != wr_کیڼ[i])
			std::بدلون(wr_ports[i], wr_ports[wr_کیڼ[i]]);
	wr_ports.resize(Get_اندازه(wr_کیڼ));
	لكل (صحيح i = 0; i < Get_اندازه(init_کیڼ); i++)
		إذا (i != init_کیڼ[i])
			std::بدلون(inits[i], inits[init_کیڼ[i]]);
	inits.resize(Get_اندازه(init_کیڼ));

	لكل (خپلکاری &پورټ : rd_ports) {
		لكل (صحيح i = 0; i < Get_اندازه(wr_کیڼ); i++) {
			پورټ.transparency_ماسک[i] = پورټ.transparency_ماسک[wr_کیڼ[i]];
			پورټ.collision_x_ماسک[i] = پورټ.collision_x_ماسک[wr_کیڼ[i]];
		}
		پورټ.transparency_ماسک.resize(Get_اندازه(wr_کیڼ));
		پورټ.collision_x_ماسک.resize(Get_اندازه(wr_کیڼ));
	}
	لكل (خپلکاری &پورټ : wr_ports) {
		لكل (صحيح i = 0; i < Get_اندازه(wr_کیڼ); i++)
			پورټ.لومړیتوب_ماسک[i] = پورټ.لومړیتوب_ماسک[wr_کیڼ[i]];
		پورټ.لومړیتوب_ماسک.resize(Get_اندازه(wr_کیڼ));
	}

	إذا (packed) {
		إذا (mem) {
			module->memories.erase(mem->name);
			ړنګول mem;
			mem = تش_اشاره;
		}
		إذا (!cell) {
			إذا (memid.تشه())
				memid = PooledName(module->design, module->twines().جمع_کړه(NEW_ID));
			cell = module->جمع_کړه_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->ځانګړتیاوې = ځانګړتیاوې;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::پلنوالی_2] = Const(پلنوالی);
		cell->parameters[ID::دننه_واټن_2] = Const(پیل_دننه_واټن);
		cell->parameters[ID::اندازه_2] = Const(اندازه);
		SigSpec rd_clk, rd_en, rd_addr, rd_معلومات;
		SigSpec wr_clk, wr_en, wr_addr, wr_معلومات;
		SigSpec rd_arst, rd_srst;
		صحيح abits = 0;
		لكل (خپلکاری &پورټ : rd_ports)
			abits = std::max(abits, Get_اندازه(پورټ.addr));
		لكل (خپلکاری &پورټ : wr_ports)
			abits = std::max(abits, Get_اندازه(پورټ.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::ویکټور<صحيح> wr_پورټ_xlat;
		لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++)
			لكل (صحيح j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_پورټ_xlat.ټېل_شا(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_ماسک_builder;
		Const::Builder rd_collision_x_ماسک_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_ارزښت_builder;
		Const::Builder rd_srst_ارزښت_builder;
		Const::Builder rd_init_ارزښت_builder;
		لكل (خپلکاری &پورټ : rd_ports) {
			لكل (خپلکاری attr: پورټ.ځانګړتیاوې)
				إذا (!cell->has_attribute(attr.لومړی))
					cell->ځانګړتیاوې.ورننباسه(attr);
			إذا (پورټ.cell) {
				module->remove(پورټ.cell);
				پورټ.cell = تش_اشاره;
			}
			لكل (صحيح sub = 0; sub < (1 << پورټ.wide_log2); sub++)
			{
				rd_wide_continuation_builder.ټېل_شا(State(sub != 0));
				rd_clk_enable_builder.ټېل_شا(State(پورټ.clk_enable));
				rd_clk_polarity_builder.ټېل_شا(State(پورټ.clk_polarity));
				rd_ce_over_srst_builder.ټېل_شا(State(پورټ.ce_over_srst));
				rd_clk.append(پورټ.clk);
				rd_arst.append(پورټ.arst);
				rd_srst.append(پورټ.srst);
				rd_en.append(پورټ.en);
				SigSpec addr = پورټ.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				rd_addr.append(addr);
				log_assert(Get_اندازه(addr) == abits);
				لكل (خپلکاری idx : wr_پورټ_xlat) {
					rd_transparency_ماسک_builder.ټېل_شا(State(منطقي(پورټ.transparency_ماسک[idx])));
					rd_collision_x_ماسک_builder.ټېل_شا(State(منطقي(پورټ.collision_x_ماسک[idx])));
				}
			}
			rd_معلومات.append(پورټ.معلومات);
			لكل (خپلکاری بټ : پورټ.arst_ارزښت)
				rd_arst_ارزښت_builder.ټېل_شا(بټ);
			لكل (خپلکاری بټ : پورټ.srst_ارزښت)
				rd_srst_ارزښت_builder.ټېل_شا(بټ);
			لكل (خپلکاری بټ : پورټ.init_ارزښت)
				rd_init_ارزښت_builder.ټېل_شا(بټ);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_ماسک = rd_transparency_ماسک_builder.build();
		Const rd_collision_x_ماسک = rd_collision_x_ماسک_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_ارزښت = rd_arst_ارزښت_builder.build();
		Const rd_srst_ارزښت = rd_srst_ارزښت_builder.build();
		Const rd_init_ارزښت = rd_init_ارزښت_builder.build();
		إذا (rd_ports.تشه()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_ارزښت = State::S0;
			rd_srst_ارزښت = State::S0;
			rd_init_ارزښت = State::S0;
		}
		إذا (rd_ports.تشه() || wr_ports.تشه()) {
			rd_transparency_ماسک = State::S0;
			rd_collision_x_ماسک = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_اندازه(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_ماسک] = rd_transparency_ماسک;
		cell->parameters[ID::RD_COLLISION_X_ماسک] = rd_collision_x_ماسک;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_ارزښت] = rd_arst_ارزښت;
		cell->parameters[ID::RD_SRST_ارزښت] = rd_srst_ارزښت;
		cell->parameters[ID::RD_INIT_ارزښت] = rd_init_ارزښت;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->سېټ_پورټ(ID::RD_CLK, rd_clk);
		cell->سېټ_پورټ(ID::RD_EN, rd_en);
		cell->سېټ_پورټ(ID::RD_ARST, rd_arst);
		cell->سېټ_پورټ(ID::RD_SRST, rd_srst);
		cell->سېټ_پورټ(ID::RD_ADDR, rd_addr);
		cell->سېټ_پورټ(ID::RD_معلومات, rd_معلومات);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_لومړیتوب_ماسک_builder;
		لكل (خپلکاری &پورټ : wr_ports) {
			لكل (خپلکاری attr: پورټ.ځانګړتیاوې)
				إذا (!cell->has_attribute(attr.لومړی))
					cell->ځانګړتیاوې.ورننباسه(attr);
			إذا (پورټ.cell) {
				module->remove(پورټ.cell);
				پورټ.cell = تش_اشاره;
			}
			لكل (صحيح sub = 0; sub < (1 << پورټ.wide_log2); sub++)
			{
				wr_wide_continuation_builder.ټېل_شا(State(sub != 0));
				wr_clk_enable_builder.ټېل_شا(State(پورټ.clk_enable));
				wr_clk_polarity_builder.ټېل_شا(State(پورټ.clk_polarity));
				wr_clk.append(پورټ.clk);
				لكل (خپلکاری idx : wr_پورټ_xlat)
					wr_لومړیتوب_ماسک_builder.ټېل_شا(State(منطقي(پورټ.لومړیتوب_ماسک[idx])));
				SigSpec addr = پورټ.sub_addr(sub);
				addr.extend_u0(abits, خطأ);
				wr_addr.append(addr);
				log_assert(Get_اندازه(addr) == abits);
			}
			wr_en.append(پورټ.en);
			wr_معلومات.append(پورټ.معلومات);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_لومړیتوب_ماسک = wr_لومړیتوب_ماسک_builder.build();
		إذا (wr_ports.تشه()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_لومړیتوب_ماسک = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_اندازه(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_لومړیتوب_ماسک] = wr_لومړیتوب_ماسک;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->سېټ_پورټ(ID::WR_CLK, wr_clk);
		cell->سېټ_پورټ(ID::WR_EN, wr_en);
		cell->سېټ_پورټ(ID::WR_ADDR, wr_addr);
		cell->سېټ_پورټ(ID::WR_معلومات, wr_معلومات);
		لكل (خپلکاری &init : inits) {
			لكل (خپلکاری attr: init.ځانګړتیاوې)
				إذا (!cell->has_attribute(attr.لومړی))
					cell->ځانګړتیاوې.ورننباسه(attr);
			إذا (init.cell) {
				module->remove(init.cell);
				init.cell = تش_اشاره;
			}
		}
		cell->parameters[ID::INIT] = get_init_معلومات();
	} وإلا {
		إذا (cell) {
			module->remove(cell);
			cell = تش_اشاره;
		}
		إذا (!mem) {
			إذا (memid.تشه())
				memid = PooledName(module->design, module->twines().جمع_کړه(NEW_ID));
			mem = module->جمع_کړه_حافظه(memid);
		}
		mem->پلنوالی = پلنوالی;
		mem->پیل_دننه_واټن = پیل_دننه_واټن;
		mem->اندازه = اندازه;
		mem->ځانګړتیاوې = ځانګړتیاوې;
		لكل (خپلکاری &پورټ : rd_ports) {
			إذا (!پورټ.cell)
				پورټ.cell = module->جمع_کړه_Cell(NEW_ID, ID($memrd_v2));
			پورټ.cell->type = ID($memrd_v2);
			پورټ.cell->ځانګړتیاوې = پورټ.ځانګړتیاوې;
			پورټ.cell->parameters[ID::MEMID] = memid.str();
			پورټ.cell->parameters[ID::ABITS] = Get_اندازه(پورټ.addr);
			پورټ.cell->parameters[ID::پلنوالی_2] = پلنوالی << پورټ.wide_log2;
			پورټ.cell->parameters[ID::CLK_ENABLE] = پورټ.clk_enable;
			پورټ.cell->parameters[ID::CLK_POLARITY] = پورټ.clk_polarity;
			پورټ.cell->parameters[ID::CE_OVER_SRST] = پورټ.ce_over_srst;
			پورټ.cell->parameters[ID::ARST_ارزښت] = پورټ.arst_ارزښت;
			پورټ.cell->parameters[ID::SRST_ارزښت] = پورټ.srst_ارزښت;
			پورټ.cell->parameters[ID::INIT_ارزښت] = پورټ.init_ارزښت;
			پورټ.cell->parameters[ID::TRANSPARENCY_ماسک] = پورټ.transparency_ماسک;
			پورټ.cell->parameters[ID::COLLISION_X_ماسک] = پورټ.collision_x_ماسک;
			پورټ.cell->parameters.erase(ID::TRANSPARENT);
			پورټ.cell->سېټ_پورټ(ID::CLK, پورټ.clk);
			پورټ.cell->سېټ_پورټ(ID::EN, پورټ.en);
			پورټ.cell->سېټ_پورټ(ID::ARST, پورټ.arst);
			پورټ.cell->سېټ_پورټ(ID::SRST, پورټ.srst);
			پورټ.cell->سېټ_پورټ(ID::ADDR, پورټ.addr);
			پورټ.cell->سېټ_پورټ(ID::معلومات_2, پورټ.معلومات);
		}
		صحيح idx = 0;
		لكل (خپلکاری &پورټ : wr_ports) {
			إذا (!پورټ.cell)
				پورټ.cell = module->جمع_کړه_Cell(NEW_ID, ID($memwr_v2));
			پورټ.cell->type = ID($memwr_v2);
			پورټ.cell->ځانګړتیاوې = پورټ.ځانګړتیاوې;
			إذا (پورټ.cell->parameters.شمېر(ID::لومړیتوب))
				پورټ.cell->parameters.erase(ID::لومړیتوب);
			پورټ.cell->parameters[ID::MEMID] = memid.str();
			پورټ.cell->parameters[ID::ABITS] = Get_اندازه(پورټ.addr);
			پورټ.cell->parameters[ID::پلنوالی_2] = پلنوالی << پورټ.wide_log2;
			پورټ.cell->parameters[ID::CLK_ENABLE] = پورټ.clk_enable;
			پورټ.cell->parameters[ID::CLK_POLARITY] = پورټ.clk_polarity;
			پورټ.cell->parameters[ID::PORTID] = idx++;
			پورټ.cell->parameters[ID::لومړیتوب_ماسک_2] = پورټ.لومړیتوب_ماسک;
			پورټ.cell->سېټ_پورټ(ID::CLK, پورټ.clk);
			پورټ.cell->سېټ_پورټ(ID::EN, پورټ.en);
			پورټ.cell->سېټ_پورټ(ID::ADDR, پورټ.addr);
			پورټ.cell->سېټ_پورټ(ID::معلومات_2, پورټ.معلومات);
		}
		idx = 0;
		لكل (خپلکاری &init : inits) {
			منطقي v2 = !init.en.is_fully_ones();
			إذا (!init.cell)
				init.cell = module->جمع_کړه_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			وإلا {
				إذا (!v2)
					init.cell->unset_پورټ(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->ځانګړتیاوې = init.ځانګړتیاوې;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_اندازه(init.addr);
			init.cell->parameters[ID::پلنوالی_2] = پلنوالی;
			init.cell->parameters[ID::ټکي] = Get_اندازه(init.معلومات) / پلنوالی;
			init.cell->parameters[ID::لومړیتوب] = idx++;
			init.cell->سېټ_پورټ(ID::ADDR, init.addr);
			init.cell->سېټ_پورټ(ID::معلومات_2, init.معلومات);
			إذا (v2)
				init.cell->سېټ_پورټ(ID::EN, init.en);
		}
	}
}

فراغ Mem::clear_inits() {
	لكل (خپلکاری &init : inits)
		init.removed = ریښتیا;
}

فراغ Mem::coalesce_inits() {
	// start address -> end address
	std::map<صحيح, صحيح> chunks;
	// Figure out chunk boundaries.
	لكل (خپلکاری &init : inits) {
		إذا (init.removed)
			استمر;
		منطقي valid = خطأ;
		لكل (خپلکاری بټ : init.en)
			إذا (بټ == State::S1)
				valid = ریښتیا;
		إذا (!valid) {
			init.removed = ریښتیا;
			استمر;
		}
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_اندازه(init.معلومات) / پلنوالی;
		خپلکاری it_e = chunks.پاسنی_bound(addr_e);
		خپلکاری it = it_e;
		طالما (it != chunks.begin()) {
			--it;
			إذا (it->دوهم < addr) {
				++it;
				اكسر;
			}
		}
		إذا (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} وإلا {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			إذا (it->لومړی < addr)
				addr = it->لومړی;
			خپلکاری it_وروستی = it_e;
			it_وروستی--;
			إذا (it_وروستی->دوهم > addr_e)
				addr_e = it_وروستی->دوهم;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<صحيح, std::ویکټور<صحيح>> inits_by_chunk;
	لكل (صحيح i = 0; i < Get_اندازه(inits); i++) {
		خپلکاری &init = inits[i];
		إذا (init.removed)
			استمر;
		خپلکاری it = chunks.پاسنی_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->لومړی].ټېل_شا(i);
		صحيح addr = init.addr.as_int();
		صحيح addr_e = addr + Get_اندازه(init.معلومات) / پلنوالی;
		log_assert(addr >= it->لومړی && addr_e <= it->دوهم);
	}
	// Process each chunk.
	لكل (خپلکاری &it : inits_by_chunk) {
		صحيح caddr = it.لومړی;
		صحيح caddr_e = chunks[caddr];
		خپلکاری &chunk_inits = it.دوهم;
		إذا (Get_اندازه(chunk_inits) == 1) {
			خپلکاری &init = inits[chunk_inits[0]];
			إذا (!init.en.is_fully_ones()) {
				لكل (صحيح i = 0; i < Get_اندازه(init.معلومات); i++)
					إذا (init.en[i % پلنوالی] != State::S1)
						init.معلومات.سېټ(i, State::Sx);
				init.en = Const(State::S1, پلنوالی);
			}
			استمر;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * پلنوالی);
		لكل (صحيح idx : chunk_inits) {
			خپلکاری &init = inits[idx];
			صحيح دننه_واټن = (init.addr.as_int() - caddr) * پلنوالی;
			log_assert(دننه_واټن >= 0);
			log_assert(دننه_واټن + Get_اندازه(init.معلومات) <= Get_اندازه(cdata));
			لكل (صحيح i = 0; i < Get_اندازه(init.معلومات); i++)
				إذا (init.en[i % پلنوالی] == State::S1)
					cdata.سېټ(i+دننه_واټن, init.معلومات[i]);
			init.removed = ریښتیا;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.معلومات = cdata;
		new_init.en = Const(State::S1, پلنوالی);
		inits.ټېل_شا(new_init);
	}
}

Const Mem::get_init_معلومات() ثابت {
	Const init_معلومات(State::Sx, پلنوالی * اندازه);
	لكل (خپلکاری &init : inits) {
		إذا (init.removed)
			استمر;
		صحيح دننه_واټن = (init.addr.as_int() - پیل_دننه_واټن) * پلنوالی;
		لكل (صحيح i = 0; i < Get_اندازه(init.معلومات); i++)
			إذا (0 <= i+دننه_واټن && i+دننه_واټن < Get_اندازه(init_معلومات) && init.en[i % پلنوالی] == State::S1)
				init_معلومات.سېټ(i+دننه_واټن, init.معلومات[i]);
	}
	أرجع init_معلومات;
}

فراغ Mem::check() {
	صحيح max_wide_log2 = 0;
	لكل (خپلکاری &پورټ : rd_ports) {
		إذا (پورټ.removed)
			استمر;
		log_assert(Get_اندازه(پورټ.clk) == 1);
		log_assert(Get_اندازه(پورټ.en) == 1);
		log_assert(Get_اندازه(پورټ.arst) == 1);
		log_assert(Get_اندازه(پورټ.srst) == 1);
		log_assert(Get_اندازه(پورټ.addr) >= پورټ.wide_log2);
		log_assert(Get_اندازه(پورټ.معلومات) == (پلنوالی << پورټ.wide_log2));
		log_assert(Get_اندازه(پورټ.init_ارزښت) == (پلنوالی << پورټ.wide_log2));
		log_assert(Get_اندازه(پورټ.arst_ارزښت) == (پلنوالی << پورټ.wide_log2));
		log_assert(Get_اندازه(پورټ.srst_ارزښت) == (پلنوالی << پورټ.wide_log2));
		إذا (!پورټ.clk_enable) {
			log_assert(پورټ.en == State::S1);
			log_assert(پورټ.arst == State::S0);
			log_assert(پورټ.srst == State::S0);
		}
		لكل (صحيح j = 0; j < پورټ.wide_log2; j++) {
			log_assert(پورټ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورټ.wide_log2);
		log_assert(Get_اندازه(پورټ.transparency_ماسک) == Get_اندازه(wr_ports));
		log_assert(Get_اندازه(پورټ.collision_x_ماسک) == Get_اندازه(wr_ports));
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++) {
			خپلکاری &wport = wr_ports[j];
			إذا ((پورټ.transparency_ماسک[j] || پورټ.collision_x_ماسک[j]) && !wport.removed) {
				log_assert(پورټ.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(پورټ.clk == wport.clk);
				log_assert(پورټ.clk_polarity == wport.clk_polarity);
			}
			log_assert(!پورټ.transparency_ماسک[j] || !پورټ.collision_x_ماسک[j]);
		}
	}
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		خپلکاری &پورټ = wr_ports[i];
		إذا (پورټ.removed)
			استمر;
		log_assert(Get_اندازه(پورټ.clk) == 1);
		log_assert(Get_اندازه(پورټ.en) == (پلنوالی << پورټ.wide_log2));
		log_assert(Get_اندازه(پورټ.معلومات) == (پلنوالی << پورټ.wide_log2));
		log_assert(Get_اندازه(پورټ.addr) >= پورټ.wide_log2);
		لكل (صحيح j = 0; j < پورټ.wide_log2; j++) {
			log_assert(پورټ.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, پورټ.wide_log2);
		log_assert(Get_اندازه(پورټ.لومړیتوب_ماسک) == Get_اندازه(wr_ports));
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++) {
			خپلکاری &wport = wr_ports[j];
			إذا (پورټ.لومړیتوب_ماسک[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(پورټ.clk_enable == wport.clk_enable);
				إذا (پورټ.clk_enable) {
					log_assert(پورټ.clk == wport.clk);
					log_assert(پورټ.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	صحيح ماسک = (1 << max_wide_log2) - 1;
	log_assert(!(پیل_دننه_واټن & ماسک));
	log_assert(!(اندازه & ماسک));
	log_assert(پلنوالی != 0);
}

نطاق {

	بنية Mem_شاخص {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_شاخص (Module *module) {
			لكل (خپلکاری cell: module->cells()) {
				إذا (cell->type.دننه(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].ورننباسه(cell);
				وإلا إذا (cell->type.دننه(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].ورننباسه(cell);
				وإلا إذا (cell->type.دننه(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].ورننباسه(cell);
			}
		}
	};

	Mem mem_from_حافظه(Module *module, RTLIL::حافظه *mem, ثابت Mem_شاخص &شاخص) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->پلنوالی, mem->پیل_دننه_واټن, mem->اندازه);
		res.packed = خطأ;
		res.mem = mem;
		res.ځانګړتیاوې = mem->ځانګړتیاوې;
		std::ویکټور<منطقي> rd_transparent;
		std::ویکټور<صحيح> wr_portid;
		إذا (شاخص.rd_ports.شمېر(memid)) {
			لكل (خپلکاری cell : شاخص.rd_ports.at(memid)) {
				MemRd mrd;
				منطقي is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.ځانګړتیاوې = cell->ځانګړتیاوې;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_پورټ(ID::CLK);
				mrd.en = cell->get_پورټ(ID::EN);
				mrd.addr = cell->get_پورټ(ID::ADDR);
				mrd.معلومات = cell->get_پورټ(ID::معلومات_2);
				mrd.wide_log2 = ceil_log2(Get_اندازه(mrd.معلومات) / mem->پلنوالی);
				منطقي transparent = خطأ;
				إذا (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = خطأ;
					mrd.arst_ارزښت = Const(State::Sx, mem->پلنوالی << mrd.wide_log2);
					mrd.srst_ارزښت = Const(State::Sx, mem->پلنوالی << mrd.wide_log2);
					mrd.init_ارزښت = Const(State::Sx, mem->پلنوالی << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					إذا (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = خطأ;
						إذا (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} وإلا {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_ارزښت = cell->parameters.at(ID::ARST_ارزښت);
					mrd.srst_ارزښت = cell->parameters.at(ID::SRST_ارزښت);
					mrd.init_ارزښت = cell->parameters.at(ID::INIT_ارزښت);
					mrd.arst = cell->get_پورټ(ID::ARST);
					mrd.srst = cell->get_پورټ(ID::SRST);
				}
				res.rd_ports.ټېل_شا(mrd);
				rd_transparent.ټېل_شا(transparent);
			}
		}
		إذا (شاخص.wr_ports.شمېر(memid)) {
			std::ویکټور<std::pair<صحيح, MemWr>> ports;
			لكل (خپلکاری cell : شاخص.wr_ports.at(memid)) {
				MemWr mwr;
				منطقي is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.ځانګړتیاوې = cell->ځانګړتیاوې;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_پورټ(ID::CLK);
				mwr.en = cell->get_پورټ(ID::EN);
				mwr.addr = cell->get_پورټ(ID::ADDR);
				mwr.معلومات = cell->get_پورټ(ID::معلومات_2);
				mwr.wide_log2 = ceil_log2(Get_اندازه(mwr.معلومات) / mem->پلنوالی);
				ports.ټېل_شا(std::make_pair(cell->parameters.at(is_compat ? ID::لومړیتوب : ID::PORTID).as_int(), mwr));
			}
			std::ترتیبول(ports.begin(), ports.پای_ټکی(), [](ثابت std::pair<صحيح, MemWr> &a, ثابت std::pair<صحيح, MemWr> &b) { أرجع a.لومړی < b.لومړی; });
			لكل (خپلکاری &it : ports) {
				res.wr_ports.ټېل_شا(it.دوهم);
				wr_portid.ټېل_شا(it.لومړی);
			}
			لكل (صحيح i = 0; i < Get_اندازه(res.wr_ports); i++) {
				خپلکاری &پورټ = res.wr_ports[i];
				منطقي is_compat = پورټ.cell->type == ID($memwr);
				إذا (is_compat) {
					پورټ.لومړیتوب_ماسک.resize(Get_اندازه(res.wr_ports));
					لكل (صحيح j = 0; j < i; j++) {
						خپلکاری &oport = res.wr_ports[j];
						إذا (پورټ.clk_enable != oport.clk_enable)
							استمر;
						إذا (پورټ.clk_enable && پورټ.clk != oport.clk)
							استمر;
						إذا (پورټ.clk_enable && پورټ.clk_polarity != oport.clk_polarity)
							استمر;
						پورټ.لومړیتوب_ماسک[j] = ریښتیا;
					}
				} وإلا {
					Const orig_prio_ماسک = پورټ.cell->parameters.at(ID::لومړیتوب_ماسک_2);
					لكل (صحيح orig_portid : wr_portid) {
						منطقي has_prio = orig_portid < Get_اندازه(orig_prio_ماسک) && orig_prio_ماسک[orig_portid] == State::S1;
						پورټ.لومړیتوب_ماسک.ټېل_شا(has_prio);
					}
				}
			}
		}
		إذا (شاخص.inits.شمېر(memid)) {
			std::ویکټور<std::pair<صحيح, MemInit>> inits;
			لكل (خپلکاری cell : شاخص.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.ځانګړتیاوې = cell->ځانګړتیاوې;
				خپلکاری addr = cell->get_پورټ(ID::ADDR);
				خپلکاری معلومات = cell->get_پورټ(ID::معلومات_2);
				إذا (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				إذا (!معلومات.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(معلومات), cell);
				init.addr = addr.as_const();
				init.معلومات = معلومات.as_const();
				إذا (cell->type == ID($meminit_v2)) {
					خپلکاری en = cell->get_پورټ(ID::EN);
					إذا (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} وإلا {
					init.en = RTLIL::Const(State::S1, mem->پلنوالی);
				}
				inits.ټېل_شا(std::make_pair(cell->parameters.at(ID::لومړیتوب).as_int(), init));
			}
			std::ترتیبول(inits.begin(), inits.پای_ټکی(), [](ثابت std::pair<صحيح, MemInit> &a, ثابت std::pair<صحيح, MemInit> &b) { أرجع a.لومړی < b.لومړی; });
			لكل (خپلکاری &it : inits)
				res.inits.ټېل_شا(it.دوهم);
		}
		لكل (صحيح i = 0; i < Get_اندازه(res.rd_ports); i++) {
			خپلکاری &پورټ = res.rd_ports[i];
			منطقي is_compat = پورټ.cell->type == ID($memrd);
			إذا (is_compat) {
				پورټ.transparency_ماسک.resize(Get_اندازه(res.wr_ports));
				پورټ.collision_x_ماسک.resize(Get_اندازه(res.wr_ports));
				إذا (!rd_transparent[i])
					استمر;
				إذا (!پورټ.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_اندازه(res.wr_ports); j++) {
					خپلکاری &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورټ.clk != wport.clk)
						استمر;
					إذا (پورټ.clk_polarity != wport.clk_polarity)
						استمر;
					پورټ.transparency_ماسک[j] = ریښتیا;
				}
			} وإلا {
				Const orig_trans_ماسک = پورټ.cell->parameters.at(ID::TRANSPARENCY_ماسک);
				Const orig_cx_ماسک = پورټ.cell->parameters.at(ID::COLLISION_X_ماسک);
				لكل (صحيح orig_portid : wr_portid) {
					پورټ.transparency_ماسک.ټېل_شا(orig_portid < Get_اندازه(orig_trans_ماسک) && orig_trans_ماسک[orig_portid] == State::S1);
					پورټ.collision_x_ماسک.ټېل_شا(orig_portid < Get_اندازه(orig_cx_ماسک) && orig_cx_ماسک[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		أرجع res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().جمع_کړه(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::پلنوالی_2).as_int(),
			cell->parameters.at(ID::دننه_واټن_2).as_int(),
			cell->parameters.at(ID::اندازه_2).as_int()
		);
		منطقي is_compat = cell->type == ID($mem);
		صحيح abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = ریښتیا;
		res.cell = cell;
		res.ځانګړتیاوې = cell->ځانګړتیاوې;
		Const &init = cell->parameters.at(ID::INIT);
		إذا (!init.is_fully_undef()) {
			صحيح pos = 0;
			طالما (pos < res.اندازه) {
				Const کلمه = init.ایستل(pos * res.پلنوالی, res.پلنوالی, State::Sx);
				إذا (کلمه.is_fully_undef()) {
					pos++;
				} وإلا {
					صحيح epos;
					لكل (epos = pos; epos < res.اندازه; epos++) {
						Const eword = init.ایستل(epos * res.پلنوالی, res.پلنوالی, State::Sx);
						إذا (eword.is_fully_undef())
							اكسر;
					}
					MemInit minit;
					minit.addr = res.پیل_دننه_واټن + pos;
					minit.معلومات = init.ایستل(pos * res.پلنوالی, (epos - pos) * res.پلنوالی, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.پلنوالی);
					res.inits.ټېل_شا(minit);
					pos = epos;
				}
			}
		}
		صحيح n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		صحيح n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		لكل (صحيح i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			طالما (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).ایستل(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).ایستل(i, 1).as_bool();
			mrd.clk = cell->get_پورټ(ID::RD_CLK).ایستل(i, 1);
			mrd.en = cell->get_پورټ(ID::RD_EN).ایستل(i, 1);
			mrd.addr = cell->get_پورټ(ID::RD_ADDR).ایستل(i * abits, abits);
			mrd.معلومات = cell->get_پورټ(ID::RD_معلومات).ایستل(i * res.پلنوالی, (ni - i) * res.پلنوالی);
			إذا (is_compat) {
				mrd.ce_over_srst = خطأ;
				mrd.arst_ارزښت = Const(State::Sx, res.پلنوالی << mrd.wide_log2);
				mrd.srst_ارزښت = Const(State::Sx, res.پلنوالی << mrd.wide_log2);
				mrd.init_ارزښت = Const(State::Sx, res.پلنوالی << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} وإلا {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).ایستل(i, 1).as_bool();
				mrd.arst_ارزښت = cell->parameters.at(ID::RD_ARST_ارزښت).ایستل(i * res.پلنوالی, (ni - i) * res.پلنوالی);
				mrd.srst_ارزښت = cell->parameters.at(ID::RD_SRST_ارزښت).ایستل(i * res.پلنوالی, (ni - i) * res.پلنوالی);
				mrd.init_ارزښت = cell->parameters.at(ID::RD_INIT_ارزښت).ایستل(i * res.پلنوالی, (ni - i) * res.پلنوالی);
				mrd.arst = cell->get_پورټ(ID::RD_ARST).ایستل(i, 1);
				mrd.srst = cell->get_پورټ(ID::RD_SRST).ایستل(i, 1);
			}
			إذا (!is_compat) {
				Const transparency_ماسک = cell->parameters.at(ID::RD_TRANSPARENCY_ماسک).ایستل(i * n_wr_ports, n_wr_ports);
				Const collision_x_ماسک = cell->parameters.at(ID::RD_COLLISION_X_ماسک).ایستل(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_ماسک.ټېل_شا(transparency_ماسک[j] == State::S1);
						mrd.collision_x_ماسک.ټېل_شا(collision_x_ماسک[j] == State::S1);
					}
			}
			res.rd_ports.ټېل_شا(mrd);
		}
		لكل (صحيح i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			طالما (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).ایستل(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).ایستل(i, 1).as_bool();
			mwr.clk = cell->get_پورټ(ID::WR_CLK).ایستل(i, 1);
			mwr.en = cell->get_پورټ(ID::WR_EN).ایستل(i * res.پلنوالی, (ni - i) * res.پلنوالی);
			mwr.addr = cell->get_پورټ(ID::WR_ADDR).ایستل(i * abits, abits);
			mwr.معلومات = cell->get_پورټ(ID::WR_معلومات).ایستل(i * res.پلنوالی, (ni - i) * res.پلنوالی);
			إذا (!is_compat) {
				Const لومړیتوب_ماسک = cell->parameters.at(ID::WR_لومړیتوب_ماسک).ایستل(i * n_wr_ports, n_wr_ports);
				لكل (صحيح j = 0; j < n_wr_ports; j++)
					إذا (wr_wide_continuation[j] != State::S1)
						mwr.لومړیتوب_ماسک.ټېل_شا(لومړیتوب_ماسک[j] == State::S1);
			}
			res.wr_ports.ټېل_شا(mwr);
		}
		إذا (is_compat) {
			لكل (صحيح i = 0; i < Get_اندازه(res.wr_ports); i++) {
				خپلکاری &پورټ = res.wr_ports[i];
				پورټ.لومړیتوب_ماسک.resize(Get_اندازه(res.wr_ports));
				لكل (صحيح j = 0; j < i; j++) {
					خپلکاری &oport = res.wr_ports[j];
					إذا (پورټ.clk_enable != oport.clk_enable)
						استمر;
					إذا (پورټ.clk_enable && پورټ.clk != oport.clk)
						استمر;
					إذا (پورټ.clk_enable && پورټ.clk_polarity != oport.clk_polarity)
						استمر;
					پورټ.لومړیتوب_ماسک[j] = ریښتیا;
				}
			}
			لكل (صحيح i = 0; i < Get_اندازه(res.rd_ports); i++) {
				خپلکاری &پورټ = res.rd_ports[i];
				پورټ.transparency_ماسک.resize(Get_اندازه(res.wr_ports));
				پورټ.collision_x_ماسک.resize(Get_اندازه(res.wr_ports));
				إذا (!cell->parameters.at(ID::RD_TRANSPARENT).ایستل(i, 1).as_bool())
					استمر;
				إذا (!پورټ.clk_enable)
					استمر;
				لكل (صحيح j = 0; j < Get_اندازه(res.wr_ports); j++) {
					خپلکاری &wport = res.wr_ports[j];
					إذا (!wport.clk_enable)
						استمر;
					إذا (پورټ.clk != wport.clk)
						استمر;
					إذا (پورټ.clk_polarity != wport.clk_polarity)
						استمر;
					پورټ.transparency_ماسک[j] = ریښتیا;
				}
			}
		}
		res.check();
		أرجع res;
	}

}

std::ویکټور<Mem> Mem::get_all_memories(Module *module) {
	std::ویکټور<Mem> res;
	Mem_شاخص شاخص(module);
	لكل (خپلکاری it: module->memories) {
		res.ټېل_شا(mem_from_حافظه(module, it.دوهم, شاخص));
	}
	لكل (خپلکاری cell: module->cells()) {
		إذا (cell->type.دننه(ID($mem), ID($mem_v2)))
			res.ټېل_شا(mem_from_cell(cell));
	}
	أرجع res;
}

std::ویکټور<Mem> Mem::get_selected_memories(Module *module) {
	std::ویکټور<Mem> res;
	Mem_شاخص شاخص(module);
	لكل (خپلکاری it: module->memories) {
		إذا (module->design->selected(module, it.دوهم))
			res.ټېل_شا(mem_from_حافظه(module, it.دوهم, شاخص));
	}
	لكل (خپلکاری cell: module->selected_cells()) {
		إذا (cell->type.دننه(ID($mem), ID($mem_v2)))
			res.ټېل_شا(mem_from_cell(cell));
	}
	أرجع res;
}

Cell *Mem::ایستل_rdff(صحيح idx, FfInitVals *initvals) {
	MemRd &پورټ = rd_ports[idx];

	إذا (!پورټ.clk_enable)
		أرجع تش_اشاره;

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
	منطقي trans_use_addr = ریښتیا;
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++)
		إذا (!پورټ.transparency_ماسک[i] && !wr_ports[i].removed)
			trans_use_addr = خطأ;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	إذا (Get_اندازه(wr_ports) == 0)
		trans_use_addr = خطأ;

	إذا (پورټ.en != State::S1 || پورټ.srst != State::S0 || پورټ.arst != State::S0 || !پورټ.init_ارزښت.is_fully_undef())
		trans_use_addr = خطأ;

	إذا (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		صحيح پلنوالی = 0;
		لكل (صحيح i = 0; i < Get_اندازه(پورټ.addr); i++)
			إذا (پورټ.addr[i].wire)
				پلنوالی++;

		إذا (پلنوالی)
		{
			SigSpec sig_q = module->جمع_کړه_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), پلنوالی);
			SigSpec sig_d;

			صحيح pos = 0;
			لكل (صحيح i = 0; i < Get_اندازه(پورټ.addr); i++)
				إذا (پورټ.addr[i].wire) {
					sig_d.append(پورټ.addr[i]);
					پورټ.addr[i] = sig_q[pos++];
				}

			c = module->جمع_کړه_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), پورټ.clk, sig_d, sig_q, پورټ.clk_polarity);
		} وإلا {
			c = تش_اشاره;
		}
	}
	وإلا
	{
		log_assert(پورټ.arst == State::S0 || پورټ.srst == State::S0);

		SigSpec async_d = module->جمع_کړه_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_اندازه(پورټ.معلومات));
		SigSpec sig_d = async_d;

		لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
			خپلکاری &wport = wr_ports[i];
			إذا (wport.removed)
				استمر;
			إذا (پورټ.transparency_ماسک[i] || پورټ.collision_x_ماسک[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == پورټ.clk);
				log_assert(wport.clk_enable == پورټ.clk_enable);
				صحيح min_wide_log2 = std::min(پورټ.wide_log2, wport.wide_log2);
				صحيح max_wide_log2 = std::max(پورټ.wide_log2, wport.wide_log2);
				منطقي wide_write = wport.wide_log2 > پورټ.wide_log2;
				لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = پورټ.addr;
					SigSpec waddr = wport.addr;
					إذا (wide_write)
						waddr = wport.sub_addr(sub);
					وإلا
						raddr = پورټ.sub_addr(sub);
					SigSpec addr_eq;
					إذا (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					صحيح pos = 0;
					صحيح ewidth = پلنوالی << min_wide_log2;
					صحيح wsub = wide_write ? sub : 0;
					صحيح rsub = wide_write ? 0 : sub;
					طالما (pos < ewidth) {
						صحيح epos = pos;
						طالما (epos < ewidth && wport.en[epos + wsub * پلنوالی] == wport.en[pos + wsub * پلنوالی])
							epos++;
						SigSpec cur = sig_d.ایستل(pos + rsub * پلنوالی, epos-pos);
						SigSpec other = پورټ.transparency_ماسک[i] ? wport.معلومات.ایستل(pos + wsub * پلنوالی, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						إذا (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * پلنوالی], addr_eq);
						وإلا
							cond = wport.en[pos + wsub * پلنوالی];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * پلنوالی, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().جمع_کړه(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_معلومات ff(module, initvals, name);
		ff.پلنوالی = Get_اندازه(پورټ.معلومات);
		ff.has_clk = ریښتیا;
		ff.sig_clk = پورټ.clk;
		ff.pol_clk = پورټ.clk_polarity;
		إذا (پورټ.en != State::S1) {
			ff.has_ce = ریښتیا;
			ff.pol_ce = ریښتیا;
			ff.sig_ce = پورټ.en;
		}
		إذا (پورټ.arst != State::S0) {
			ff.has_arst = ریښتیا;
			ff.pol_arst = ریښتیا;
			ff.sig_arst = پورټ.arst;
			ff.val_arst = پورټ.arst_ارزښت;
		}
		إذا (پورټ.srst != State::S0) {
			ff.has_srst = ریښتیا;
			ff.pol_srst = ریښتیا;
			ff.sig_srst = پورټ.srst;
			ff.val_srst = پورټ.srst_ارزښت;
			ff.ce_over_srst = ff.has_ce && پورټ.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = پورټ.معلومات;
		ff.val_init = پورټ.init_ارزښت;
		پورټ.معلومات = async_d;
		c = ff.emit();
	}

	إذا (c)
		لوګاریتم("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	پورټ.en = State::S1;
	پورټ.clk = State::S0;
	پورټ.arst = State::S0;
	پورټ.srst = State::S0;
	پورټ.clk_enable = خطأ;
	پورټ.clk_polarity = ریښتیا;
	پورټ.ce_over_srst = خطأ;
	پورټ.arst_ارزښت = Const(State::Sx, Get_اندازه(پورټ.معلومات));
	پورټ.srst_ارزښت = Const(State::Sx, Get_اندازه(پورټ.معلومات));
	پورټ.init_ارزښت = Const(State::Sx, Get_اندازه(پورټ.معلومات));

	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		پورټ.transparency_ماسک[i] = خطأ;
		پورټ.collision_x_ماسک[i] = خطأ;
	}

	أرجع c;
}

فراغ Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::ویکټور<MemRd> new_rd_ports;
	std::ویکټور<MemWr> new_wr_ports;
	std::ویکټور<std::pair<صحيح, صحيح>> new_rd_map;
	std::ویکټور<std::pair<صحيح, صحيح>> new_wr_map;
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خپلکاری &پورټ = rd_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورټ.wide_log2); sub++) {
			new_rd_map.ټېل_شا(std::make_pair(i, sub));
		}
	}
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		خپلکاری &پورټ = wr_ports[i];
		لكل (صحيح sub = 0; sub < (1 << پورټ.wide_log2); sub++) {
			new_wr_map.ټېل_شا(std::make_pair(i, sub));
		}
	}
	لكل (خپلکاری &it : new_rd_map) {
		MemRd &orig = rd_ports[it.لومړی];
		MemRd پورټ = orig;
		إذا (it.دوهم != 0)
			پورټ.cell = تش_اشاره;
		إذا (پورټ.wide_log2) {
			پورټ.معلومات = پورټ.معلومات.ایستل(it.دوهم * پلنوالی, پلنوالی);
			پورټ.init_ارزښت = پورټ.init_ارزښت.ایستل(it.دوهم * پلنوالی, پلنوالی);
			پورټ.arst_ارزښت = پورټ.arst_ارزښت.ایستل(it.دوهم * پلنوالی, پلنوالی);
			پورټ.srst_ارزښت = پورټ.srst_ارزښت.ایستل(it.دوهم * پلنوالی, پلنوالی);
			پورټ.addr = پورټ.sub_addr(it.دوهم);
			پورټ.wide_log2 = 0;
		}
		پورټ.transparency_ماسک.clear();
		پورټ.collision_x_ماسک.clear();
		لكل (خپلکاری &it2 : new_wr_map)
			پورټ.transparency_ماسک.ټېل_شا(orig.transparency_ماسک[it2.لومړی]);
		لكل (خپلکاری &it2 : new_wr_map)
			پورټ.collision_x_ماسک.ټېل_شا(orig.collision_x_ماسک[it2.لومړی]);
		new_rd_ports.ټېل_شا(پورټ);
	}
	لكل (خپلکاری &it : new_wr_map) {
		MemWr &orig = wr_ports[it.لومړی];
		MemWr پورټ = orig;
		إذا (it.دوهم != 0)
			پورټ.cell = تش_اشاره;
		إذا (پورټ.wide_log2) {
			پورټ.معلومات = پورټ.معلومات.ایستل(it.دوهم * پلنوالی, پلنوالی);
			پورټ.en = پورټ.en.ایستل(it.دوهم * پلنوالی, پلنوالی);
			پورټ.addr = پورټ.sub_addr(it.دوهم);
			پورټ.wide_log2 = 0;
		}
		پورټ.لومړیتوب_ماسک.clear();
		لكل (خپلکاری &it2 : new_wr_map)
			پورټ.لومړیتوب_ماسک.ټېل_شا(orig.لومړیتوب_ماسک[it2.لومړی]);
		new_wr_ports.ټېل_شا(پورټ);
	}
	std::بدلون(rd_ports, new_rd_ports);
	std::بدلون(wr_ports, new_wr_ports);
}

فراغ Mem::emulate_لومړیتوب(صحيح idx1, صحيح idx2, FfInitVals *initvals)
{
	خپلکاری &port1 = wr_ports[idx1];
	خپلکاری &port2 = wr_ports[idx2];
	إذا (!port2.لومړیتوب_ماسک[idx1])
		أرجع;
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خپلکاری &rport = rd_ports[i];
		إذا (rport.removed)
			استمر;
		إذا (rport.transparency_ماسک[idx1] && !(rport.transparency_ماسک[idx2] || rport.collision_x_ماسک[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	صحيح min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	صحيح max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	منطقي wide1 = port1.wide_log2 > port2.wide_log2;
	لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		إذا (wide1)
			addr1 = port1.sub_addr(sub);
		وإلا
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		صحيح ewidth = پلنوالی << min_wide_log2;
		صحيح sub1 = wide1 ? sub : 0;
		صحيح sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_بټ, Sig_بټ>, Sig_بټ> cache;
		لكل (صحيح pos = 0; pos < ewidth; pos++) {
			Sig_بټ &en1 = port1.en[pos + sub1 * پلنوالی];
			Sig_بټ &en2 = port2.en[pos + sub2 * پلنوالی];
			std::pair<Sig_بټ, Sig_بټ> کلی(en1, en2);
			إذا (cache.شمېر(کلی)) {
				en1 = cache[کلی];
			} وإلا {
				Sig_بټ active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_بټ nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[کلی] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.لومړیتوب_ماسک[idx1] = خطأ;
}

فراغ Mem::emulate_transparency(صحيح widx, صحيح ridx, FfInitVals *initvals) {
	خپلکاری &wport = wr_ports[widx];
	خپلکاری &rport = rd_ports[ridx];
	log_assert(rport.transparency_ماسک[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	لكل (صحيح i = Get_اندازه(wr_ports) - 1; i > widx; i--) {
		إذا (wr_ports[i].removed)
			استمر;
		إذا (rport.transparency_ماسک[i] && wr_ports[i].لومړیتوب_ماسک[widx])
			emulate_transparency(i, ridx, initvals);
	}
	صحيح min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	صحيح max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	منطقي wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(wport.معلومات));
	module->جمع_کړه_Dffe(NEW_ID, rport.clk, rport.en, wport.معلومات, wdata_q, rport.clk_polarity, ریښتیا);
	لكل (صحيح sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		لكل (صحيح j = min_wide_log2; j < max_wide_log2; j++)
			إذا (wide_write)
				waddr = wport.sub_addr(sub);
			وإلا
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		إذا (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		صحيح pos = 0;
		صحيح ewidth = پلنوالی << min_wide_log2;
		صحيح wsub = wide_write ? sub : 0;
		صحيح rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->جمع_کړه_Wire(NEW_ID, ewidth);
		طالما (pos < ewidth) {
			صحيح epos = pos;
			طالما (epos < ewidth && wport.en[epos + wsub * پلنوالی] == wport.en[pos + wsub * پلنوالی])
				epos++;
			SigSpec cond;
			إذا (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * پلنوالی], addr_eq);
			وإلا
				cond = wport.en[pos + wsub * پلنوالی];
			SigSpec cond_q = module->جمع_کړه_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_معلومات ff(module, initvals, NEW_ID);
			ff.پلنوالی = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = ریښتیا;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			إذا (rport.en != State::S1) {
				ff.has_ce = ریښتیا;
				ff.sig_ce = rport.en;
				ff.pol_ce = ریښتیا;
			}
			إذا (rport.arst != State::S0) {
				ff.has_arst = ریښتیا;
				ff.sig_arst = rport.arst;
				ff.pol_arst = ریښتیا;
				ff.val_arst = State::S0;
			}
			إذا (rport.srst != State::S0) {
				ff.has_srst = ریښتیا;
				ff.sig_srst = rport.srst;
				ff.pol_srst = ریښتیا;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			إذا (!rport.init_ارزښت.is_fully_undef())
				ff.val_init = State::S0;
			وإلا
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.ایستل(pos, epos-pos);
			SigSpec other = wdata_q.ایستل(pos + wsub * پلنوالی, epos-pos);
			SigSpec dest = rport.معلومات.ایستل(pos + rsub * پلنوالی, epos-pos);
			module->جمع_کړه_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.معلومات.replace(rsub * پلنوالی, rdata_a);
	}
	rport.transparency_ماسک[widx] = خطأ;
	rport.collision_x_ماسک[widx] = ریښتیا;
}

فراغ Mem::prepare_wr_یوځای(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	خپلکاری &port1 = wr_ports[idx1];
	خپلکاری &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	لكل (صحيح i = 0; i < idx1; i++)
		إذا (port2.لومړیتوب_ماسک[i])
			port1.لومړیتوب_ماسک[i] = ریښتیا;
	// If port 2 has priority over a port after port 1, emulate it.
	لكل (صحيح i = idx1 + 1; i < idx2; i++)
		إذا (port2.لومړیتوب_ماسک[i] && !wr_ports[i].removed)
			emulate_لومړیتوب(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	لكل (صحيح i = idx2 + 1; i < Get_اندازه(wr_ports); i++) {
		خپلکاری &oport = wr_ports[i];
		إذا (oport.لومړیتوب_ماسک[idx2])
			oport.لومړیتوب_ماسک[idx1] = ریښتیا;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++) {
		خپلکاری &rport = rd_ports[i];
		إذا (rport.removed)
			استمر;
		// If collision already undefined with both ports, it's fine.
		إذا (rport.collision_x_ماسک[idx1] && rport.collision_x_ماسک[idx2])
			استمر;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		إذا (rport.collision_x_ماسک[idx1]) {
			rport.collision_x_ماسک[idx1] = خطأ;
			rport.transparency_ماسک[idx1] = rport.transparency_ماسک[idx2];
			استمر;
		}
		إذا (rport.collision_x_ماسک[idx2]) {
			rport.collision_x_ماسک[idx2] = خطأ;
			rport.transparency_ماسک[idx2] = rport.transparency_ماسک[idx1];
			استمر;
		}
		// If transparent with both ports, also fine.
		إذا (rport.transparency_ماسک[idx1] && rport.transparency_ماسک[idx2])
			استمر;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		إذا (rport.transparency_ماسک[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_ماسک[idx1] = خطأ;
			استمر;
		}
		إذا (rport.transparency_ماسک[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_ماسک[idx2] = خطأ;
			استمر;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

فراغ Mem::prepare_rd_یوځای(صحيح idx1, صحيح idx2, FfInitVals *initvals) {
	خپلکاری &port1 = rd_ports[idx1];
	خپلکاری &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	لكل (صحيح i = 0; i < Get_اندازه(wr_ports); i++) {
		إذا (wr_ports[i].removed)
			استمر;
		// Both ports undefined, OK.
		إذا (port1.collision_x_ماسک[i] && port2.collision_x_ماسک[i])
			استمر;
		// Only one port undefined — change its behavior
		// to align with the other port.
		إذا (port1.collision_x_ماسک[i]) {
			port1.collision_x_ماسک[i] = خطأ;
			port1.transparency_ماسک[i] = port2.transparency_ماسک[i];
			استمر;
		}
		إذا (port2.collision_x_ماسک[i]) {
			port2.collision_x_ماسک[i] = خطأ;
			port2.transparency_ماسک[i] = port1.transparency_ماسک[i];
			استمر;
		}
		// Both ports transparent, OK.
		إذا (port1.transparency_ماسک[i] && port2.transparency_ماسک[i])
			استمر;
		// Only one port transparent — emulate transparency
		// on the other.
		إذا (port1.transparency_ماسک[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_ماسک[i] = خطأ;
			استمر;
		}
		إذا (port2.transparency_ماسک[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_ماسک[i] = خطأ;
			استمر;
		}
		// No ports transparent, OK.
	}

}

فراغ Mem::widen_prep(صحيح wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	صحيح ماسک = ((1 << wide_log2) - 1);
	صحيح delta = پیل_دننه_واټن & ماسک;
	پیل_دننه_واټن -= delta;
	اندازه += delta;
	إذا (اندازه & ماسک) {
		اندازه |= ماسک;
		اندازه++;
	}
}

فراغ Mem::widen_wr_پورټ(صحيح idx, صحيح wide_log2) {
	widen_prep(wide_log2);
	خپلکاری &پورټ = wr_ports[idx];
	log_assert(پورټ.wide_log2 <= wide_log2);
	إذا (پورټ.wide_log2 < wide_log2) {
		SigSpec new_معلومات, new_en;
		SigSpec addr_lo = پورټ.addr.ایستل(0, wide_log2);
		لكل (صحيح sub = 0; sub < (1 << wide_log2); sub += (1 << پورټ.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			إذا (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_معلومات.append(پورټ.معلومات);
				new_en.append(پورټ.en);
			} وإلا إذا (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_معلومات.append(Const(State::Sx, Get_اندازه(پورټ.معلومات)));
				new_en.append(Const(State::S0, Get_اندازه(پورټ.معلومات)));
			} وإلا {
				// May or may not write to this subword.
				new_معلومات.append(پورټ.معلومات);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_اندازه(پورټ.معلومات)), پورټ.en, addr_eq);
				new_en.append(en);
			}
		}
		پورټ.addr.replace(پورټ.wide_log2, Const(State::S0, wide_log2 - پورټ.wide_log2));
		پورټ.معلومات = new_معلومات;
		پورټ.en = new_en;
		پورټ.wide_log2 = wide_log2;
	}
}

فراغ Mem::emulate_rden(صحيح idx, FfInitVals *initvals) {
	خپلکاری &پورټ = rd_ports[idx];
	log_assert(پورټ.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_معلومات = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.معلومات));
	Wire *prev_معلومات = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.معلومات));
	Wire *sel = module->جمع_کړه_Wire(NEW_ID);
	Ff_معلومات ff_sel(module, initvals, NEW_ID);
	Ff_معلومات ff_معلومات(module, initvals, NEW_ID);
	ff_sel.پلنوالی = 1;
	ff_sel.has_clk = ریښتیا;
	ff_sel.sig_clk = پورټ.clk;
	ff_sel.pol_clk = پورټ.clk_polarity;
	ff_sel.sig_d = پورټ.en;
	ff_sel.sig_q = sel;
	ff_معلومات.پلنوالی = Get_اندازه(پورټ.معلومات);
	ff_معلومات.has_clk = ریښتیا;
	ff_معلومات.sig_clk = پورټ.clk;
	ff_معلومات.pol_clk = پورټ.clk_polarity;
	ff_معلومات.sig_d = پورټ.معلومات;
	ff_معلومات.sig_q = prev_معلومات;
	إذا (!پورټ.init_ارزښت.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_معلومات.val_init = پورټ.init_ارزښت;
		پورټ.init_ارزښت = Const(State::Sx, Get_اندازه(پورټ.معلومات));
	} وإلا {
		ff_sel.val_init = State::Sx;
		ff_معلومات.val_init = Const(State::Sx, Get_اندازه(پورټ.معلومات));
	}
	إذا (پورټ.arst != State::S0) {
		ff_sel.has_arst = ریښتیا;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = پورټ.arst;
		ff_sel.pol_arst = ریښتیا;
		ff_معلومات.has_arst = ریښتیا;
		ff_معلومات.val_arst = پورټ.arst_ارزښت;
		ff_معلومات.sig_arst = پورټ.arst;
		ff_معلومات.pol_arst = ریښتیا;
		پورټ.arst = State::S0;
	}
	إذا (پورټ.srst != State::S0) {
		log_assert(!پورټ.ce_over_srst);
		ff_sel.has_srst = ریښتیا;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = پورټ.srst;
		ff_sel.pol_srst = ریښتیا;
		ff_sel.ce_over_srst = خطأ;
		ff_معلومات.has_srst = ریښتیا;
		ff_معلومات.val_srst = پورټ.srst_ارزښت;
		ff_معلومات.sig_srst = پورټ.srst;
		ff_معلومات.pol_srst = ریښتیا;
		ff_معلومات.ce_over_srst = خطأ;
		پورټ.srst = State::S0;
	}
	ff_sel.emit();
	ff_معلومات.emit();
	module->جمع_کړه_Mux(NEW_ID, prev_معلومات, new_معلومات, sel, پورټ.معلومات);
	پورټ.معلومات = new_معلومات;
	پورټ.en = State::S1;
}

فراغ Mem::emulate_reset(صحيح idx, منطقي emu_init, منطقي emu_arst, منطقي emu_srst, FfInitVals *initvals) {
	خپلکاری &پورټ = rd_ports[idx];
	إذا (emu_init && !پورټ.init_ارزښت.is_fully_undef()) {
		Wire *sel = module->جمع_کړه_Wire(NEW_ID);
		Ff_معلومات ff_sel(module, initvals, NEW_ID);
		Wire *new_معلومات = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.معلومات));
		ff_sel.پلنوالی = 1;
		ff_sel.has_clk = ریښتیا;
		ff_sel.sig_clk = پورټ.clk;
		ff_sel.pol_clk = پورټ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		إذا (پورټ.en != State::S1) {
			ff_sel.has_ce = ریښتیا;
			ff_sel.sig_ce = پورټ.en;
			ff_sel.pol_ce = ریښتیا;
			ff_sel.ce_over_srst = پورټ.ce_over_srst;
		}
		إذا (پورټ.arst != State::S0) {
			ff_sel.has_arst = ریښتیا;
			ff_sel.sig_arst = پورټ.arst;
			ff_sel.pol_arst = ریښتیا;
			إذا (emu_arst && پورټ.arst_ارزښت == پورټ.init_ارزښت) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				پورټ.arst = State::S0;
			} وإلا {
				ff_sel.val_arst = State::S1;
			}
		}
		إذا (پورټ.srst != State::S0) {
			ff_sel.has_srst = ریښتیا;
			ff_sel.sig_srst = پورټ.srst;
			ff_sel.pol_srst = ریښتیا;
			إذا (emu_srst && پورټ.srst_ارزښت == پورټ.init_ارزښت) {
				ff_sel.val_srst = State::S0;
				پورټ.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جمع_کړه_Mux(NEW_ID, پورټ.init_ارزښت, new_معلومات, sel, پورټ.معلومات);
		پورټ.معلومات = new_معلومات;
		پورټ.init_ارزښت = Const(State::Sx, Get_اندازه(پورټ.معلومات));
	}
	إذا (emu_arst && پورټ.arst != State::S0) {
		Wire *sel = module->جمع_کړه_Wire(NEW_ID);
		Ff_معلومات ff_sel(module, initvals, NEW_ID);
		Wire *new_معلومات = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.معلومات));
		ff_sel.پلنوالی = 1;
		ff_sel.has_clk = ریښتیا;
		ff_sel.sig_clk = پورټ.clk;
		ff_sel.pol_clk = پورټ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورټ.init_ارزښت.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورټ.en != State::S1) {
			ff_sel.has_ce = ریښتیا;
			ff_sel.sig_ce = پورټ.en;
			ff_sel.pol_ce = ریښتیا;
			ff_sel.ce_over_srst = پورټ.ce_over_srst;
		}
		ff_sel.has_arst = ریښتیا;
		ff_sel.sig_arst = پورټ.arst;
		ff_sel.pol_arst = ریښتیا;
		ff_sel.val_arst = State::S0;
		إذا (پورټ.srst != State::S0) {
			ff_sel.has_srst = ریښتیا;
			ff_sel.sig_srst = پورټ.srst;
			ff_sel.pol_srst = ریښتیا;
			إذا (emu_srst && پورټ.srst_ارزښت == پورټ.arst_ارزښت) {
				ff_sel.val_srst = State::S0;
				پورټ.srst = State::S0;
			} وإلا {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->جمع_کړه_Mux(NEW_ID, پورټ.arst_ارزښت, new_معلومات, sel, پورټ.معلومات);
		پورټ.معلومات = new_معلومات;
		پورټ.arst = State::S0;
	}
	إذا (emu_srst && پورټ.srst != State::S0) {
		Wire *sel = module->جمع_کړه_Wire(NEW_ID);
		Ff_معلومات ff_sel(module, initvals, NEW_ID);
		Wire *new_معلومات = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.معلومات));
		ff_sel.پلنوالی = 1;
		ff_sel.has_clk = ریښتیا;
		ff_sel.sig_clk = پورټ.clk;
		ff_sel.pol_clk = پورټ.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		إذا (پورټ.init_ارزښت.is_fully_undef())
			ff_sel.val_init = State::Sx;
		وإلا
			ff_sel.val_init = State::S1;
		إذا (پورټ.en != State::S1) {
			ff_sel.has_ce = ریښتیا;
			ff_sel.sig_ce = پورټ.en;
			ff_sel.pol_ce = ریښتیا;
			ff_sel.ce_over_srst = پورټ.ce_over_srst;
		}
		ff_sel.has_srst = ریښتیا;
		ff_sel.sig_srst = پورټ.srst;
		ff_sel.pol_srst = ریښتیا;
		ff_sel.val_srst = State::S0;
		إذا (پورټ.arst != State::S0) {
			ff_sel.has_arst = ریښتیا;
			ff_sel.sig_arst = پورټ.arst;
			ff_sel.pol_arst = ریښتیا;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->جمع_کړه_Mux(NEW_ID, پورټ.srst_ارزښت, new_معلومات, sel, پورټ.معلومات);
		پورټ.معلومات = new_معلومات;
		پورټ.srst = State::S0;
	}
}

فراغ Mem::emulate_rd_ce_over_srst(صحيح idx) {
	خپلکاری &پورټ = rd_ports[idx];
	log_assert(پورټ.clk_enable);
	إذا (پورټ.en == State::S1 || پورټ.srst == State::S0 || !پورټ.ce_over_srst) {
		پورټ.ce_over_srst = خطأ;
		أرجع;
	}
	پورټ.ce_over_srst = خطأ;
	پورټ.srst = module->And(NEW_ID, پورټ.en, پورټ.srst);
}

فراغ Mem::emulate_rd_srst_over_ce(صحيح idx) {
	خپلکاری &پورټ = rd_ports[idx];
	log_assert(پورټ.clk_enable);
	إذا (پورټ.en == State::S1 || پورټ.srst == State::S0 || پورټ.ce_over_srst) {
		پورټ.ce_over_srst = ریښتیا;
		أرجع;
	}
	پورټ.ce_over_srst = ریښتیا;
	پورټ.en = module->Or(NEW_ID, پورټ.en, پورټ.srst);
}

منطقي Mem::emulate_read_لومړی_ok() {
	إذا (wr_ports.تشه())
		أرجع خطأ;
	SigSpec clk = wr_ports[0].clk;
	منطقي clk_polarity = wr_ports[0].clk_polarity;
	لكل (خپلکاری &پورټ: wr_ports) {
		إذا (!پورټ.clk_enable)
			أرجع خطأ;
		إذا (پورټ.clk != clk)
			أرجع خطأ;
		إذا (پورټ.clk_polarity != clk_polarity)
			أرجع خطأ;
	}
	منطقي وموندل_read_لومړی = خطأ;
	لكل (خپلکاری &پورټ: rd_ports) {
		إذا (!پورټ.clk_enable)
			أرجع خطأ;
		إذا (پورټ.clk != clk)
			أرجع خطأ;
		إذا (پورټ.clk_polarity != clk_polarity)
			أرجع خطأ;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++)
			إذا (!پورټ.transparency_ماسک[j] && !پورټ.collision_x_ماسک[j])
				وموندل_read_لومړی = ریښتیا;
	}
	أرجع وموندل_read_لومړی;
}

فراغ Mem::emulate_read_لومړی(FfInitVals *initvals) {
	log_assert(emulate_read_لومړی_ok());
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++)
			إذا (rd_ports[i].transparency_ماسک[j])
				emulate_transparency(j, i, initvals);
	لكل (صحيح i = 0; i < Get_اندازه(rd_ports); i++)
		لكل (صحيح j = 0; j < Get_اندازه(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_ماسک[j]);
			rd_ports[i].collision_x_ماسک[j] = خطأ;
			rd_ports[i].transparency_ماسک[j] = ریښتیا;
		}
	لكل (خپلکاری &پورټ: wr_ports) {
		Wire *new_معلومات = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.معلومات));
		Wire *new_addr = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(پورټ.addr));
		خپلکاری compressed = پورټ.compress_en();
		Wire *new_en = module->جمع_کړه_Wire(NEW_ID, Get_اندازه(compressed.لومړی));
		Ff_معلومات ff_معلومات(module, initvals, NEW_ID);
		Ff_معلومات ff_addr(module, initvals, NEW_ID);
		Ff_معلومات ff_en(module, initvals, NEW_ID);
		ff_معلومات.پلنوالی = Get_اندازه(پورټ.معلومات);
		ff_معلومات.has_clk = ریښتیا;
		ff_معلومات.sig_clk = پورټ.clk;
		ff_معلومات.pol_clk = پورټ.clk_polarity;
		ff_معلومات.sig_d = پورټ.معلومات;
		ff_معلومات.sig_q = new_معلومات;;
		ff_معلومات.val_init = Const(State::Sx, ff_معلومات.پلنوالی);
		ff_معلومات.emit();
		ff_addr.پلنوالی = Get_اندازه(پورټ.addr);
		ff_addr.has_clk = ریښتیا;
		ff_addr.sig_clk = پورټ.clk;
		ff_addr.pol_clk = پورټ.clk_polarity;
		ff_addr.sig_d = پورټ.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.پلنوالی);
		ff_addr.emit();
		ff_en.پلنوالی = Get_اندازه(compressed.لومړی);
		ff_en.has_clk = ریښتیا;
		ff_en.sig_clk = پورټ.clk;
		ff_en.pol_clk = پورټ.clk_polarity;
		ff_en.sig_d = compressed.لومړی;
		ff_en.sig_q = new_en;;
		إذا (inits.تشه())
			ff_en.val_init = Const(State::Sx, ff_en.پلنوالی);
		وإلا
			ff_en.val_init = Const(State::S0, ff_en.پلنوالی);
		ff_en.emit();
		پورټ.معلومات = new_معلومات;
		پورټ.addr = new_addr;
		پورټ.en = پورټ.decompress_en(compressed.دوهم, new_en);
	}
}

std::pair<SigSpec, std::ویکټور<صحيح>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::ویکټور<صحيح> swizzle;
	Sig_بټ prev_بټ = en[0];
	صحيح idx = 0;
	لكل (خپلکاری &بټ: en) {
		إذا (بټ != prev_بټ) {
			sig.append(بټ);
			prev_بټ = بټ;
			idx++;
		}
		swizzle.ټېل_شا(idx);
	}
	log_assert(idx + 1 == Get_اندازه(sig));
	أرجع {sig, swizzle};
}

SigSpec MemWr::decompress_en(ثابت std::ویکټور<صحيح> &swizzle, SigSpec sig) {
	SigSpec res;
	لكل (صحيح i: swizzle)
		res.append(sig[i]);
	أرجع res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->اندازه), mem->پلنوالی)
{
	لكل(ثابت خپلکاری &init : mem->inits) {
		إذا(init.en.is_fully_zero()) استمر;
		log_assert(init.en.اندازه() == معلومات_پلنوالی_2);
		إذا(init.en.is_fully_ones())
			ورننباسه_concatenated(init.addr.as_int(), init.معلومات);
		وإلا {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t ټکي_2 = init.معلومات.اندازه() / معلومات_پلنوالی_2;
			RTLIL::Const معلومات = init.معلومات;
			log_assert(معلومات.اندازه() % معلومات_پلنوالی_2 == 0);
			لكل(addr_t i = 0; i < ټکي_2; i++) {
				RTLIL::Const پخوانی = (*هذا)[addr + i];
				لكل(صحيح j = 0; j < معلومات_پلنوالی_2; j++)
					إذا(init.en[j] != State::S1)
						معلومات.سېټ(معلومات_پلنوالی_2 * i + j, پخوانی[j]);
			}
			ورننباسه_concatenated(init.addr.as_int(), معلومات);
		}
	}
}

MemContents::iterator & MemContents::iterator::چلوونکی++() {
	خپلکاری it = حافظه_2->ارزښتونه_2.پاسنی_bound(_addr);
	إذا(it == حافظه_2->ارزښتونه_2.پای_ټکی()) {
		حافظه_2 = تش_اشاره;
		_addr = ~(addr_t) 0;
	} وإلا
		_addr = it->لومړی;
	أرجع *هذا;
}

فراغ MemContents::check() {
	log_assert(addr_پلنوالی_2 > 0 && addr_پلنوالی_2 < (صحيح)حجم(addr_t) * 8);
	log_assert(معلومات_پلنوالی_2 > 0);
	log_assert(default_ارزښت.اندازه() == معلومات_پلنوالی_2);
	إذا(ارزښتونه_2.تشه()) أرجع;
	خپلکاری it = ارزښتونه_2.begin();
	لكل(;;) {
		log_assert(!it->دوهم.تشه());
		log_assert(it->دوهم.اندازه() % معلومات_پلنوالی_2 == 0);
		خپلکاری end1 = range_پای_ټکی(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_پلنوالی_2));
		log_assert(end1 <= (addr_t)(1<<addr_پلنوالی_2));
		إذا(++it == ارزښتونه_2.پای_ټکی())
			اكسر;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

منطقي MemContents::range_لري(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ثابت {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	أرجع it != ارزښتونه_2.پای_ټکی() && addr - _range_begin(it) < range_اندازه(it);
}


منطقي MemContents::range_لري(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t پای_ټکی_addr) ثابت {
	// note that we assume begin_addr <= end_addr
	أرجع it != ارزښتونه_2.پای_ټکی() && _range_begin(it) <= begin_addr && پای_ټکی_addr - _range_begin(it) <= range_اندازه(it);
}

منطقي MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t پای_ټکی_addr) ثابت {
	إذا(it == ارزښتونه_2.پای_ټکی() || begin_addr >= پای_ټکی_addr)
		أرجع خطأ;
	خپلکاری top1 = range_پای_ټکی(it) - 1;
	خپلکاری top2 = پای_ټکی_addr - 1;
	أرجع !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) ثابت {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_پلنوالی_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	خپلکاری it = const_cast<std::map<addr_t, RTLIL::Const> &>(ارزښتونه_2).پاسنی_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	إذا(it == ارزښتونه_2.begin())
		أرجع it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	خپلکاری it_prev = std::بل(it, -1);
	إذا(range_لري(it_prev, addr))
		أرجع it_prev;
	وإلا
		أرجع it;
}

RTLIL::Const MemContents::چلوونکی[](addr_t addr) ثابت {
	خپلکاری it = _range_at(addr);
	إذا(range_لري(it, addr))
		أرجع it->دوهم.ایستل(range_دننه_واټن(it, addr), معلومات_پلنوالی_2);
	وإلا
		أرجع default_ارزښت;
}

MemContents::addr_t MemContents::شمېر_range(addr_t begin_addr, addr_t پای_ټکی_addr) ثابت {
	addr_t شمېر = 0;
	لكل(خپلکاری it = _range_at(begin_addr); _range_overlaps(it, begin_addr, پای_ټکی_addr); it++) {
		خپلکاری لومړی = std::max(_range_begin(it), begin_addr);
		خپلکاری وروستی = std::min(range_پای_ټکی(it), پای_ټکی_addr);
		شمېر += وروستی - لومړی;
	}
	أرجع شمېر;
}

فراغ MemContents::clear_range(addr_t begin_addr, addr_t پای_ټکی_addr) {
	إذا(begin_addr >= پای_ټکی_addr) أرجع;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	خپلکاری begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	خپلکاری پای_ټکی_it = ارزښتونه_2.پاسنی_bound(پای_ټکی_addr - 1);
	إذا(begin_it == پای_ټکی_it)
		أرجع; // nothing to do
	// the last iterator affected is one before the first one not affected
	خپلکاری وروستی_it = std::بل(پای_ټکی_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	إذا(range_لري(وروستی_it, پای_ټکی_addr - 1)) {
		خپلکاری new_begin = پای_ټکی_addr;
		خپلکاری پای_ټکی = range_پای_ټکی(وروستی_it);
		// if there is data past the end address, preserve it by creating a new range
		إذا(new_begin != پای_ټکی)
			پای_ټکی_it = ارزښتونه_2.emplace_hint(وروستی_it, new_begin, وروستی_it->دوهم.ایستل(range_دننه_واټن(وروستی_it, new_begin), (range_پای_ټکی(وروستی_it) - new_begin) * معلومات_پلنوالی_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	إذا(range_لري(begin_it, begin_addr)) {
		خپلکاری new_پای_ټکی = begin_addr;
		// if there is data before the start address, truncate but don't delete
		إذا(new_پای_ټکی != begin_it->لومړی) {
			begin_it->دوهم.extu(range_دننه_واټن(begin_it, new_پای_ټکی));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	ارزښتونه_2.erase(begin_it, پای_ټکی_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t پای_ټکی_addr) {
	إذا(begin_addr >= پای_ټکی_addr)
		أرجع ارزښتونه_2.پای_ټکی(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	خپلکاری ښکتنی_it = begin_addr == 0 ? ارزښتونه_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	إذا (range_لري(ښکتنی_it, begin_addr, پای_ټکی_addr))
		أرجع ښکتنی_it;
	// find the first range containing any addr >= end_addr
	خپلکاری پاسنی_it = _range_at(پای_ټکی_addr);
	// check if either of the two ranges we just found touch our range
	منطقي ښکتنی_touch = begin_addr > 0 && range_لري(ښکتنی_it, begin_addr - 1);
	منطقي پاسنی_touch = range_لري(پاسنی_it, پای_ټکی_addr);
	إذا (ښکتنی_touch && پاسنی_touch) {
		log_assert (ښکتنی_it != پاسنی_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		خپلکاری پاسنی_پای_ټکی = range_پای_ټکی(پاسنی_it);
		// make range bigger (maybe reserve here instead of resize?)
		ښکتنی_it->دوهم.resize(range_دننه_واټن(ښکتنی_it, پاسنی_پای_ټکی), State::Sx);
		// copy only the data beyond our range
		std::copy(range_معلومات(پاسنی_it, پای_ټکی_addr), range_معلومات(پاسنی_it, پاسنی_پای_ټکی), range_معلومات(ښکتنی_it, پای_ټکی_addr));
		// keep lower_it, but delete upper_it
		ارزښتونه_2.erase(std::بل(ښکتنی_it), std::بل(پاسنی_it));
		أرجع ښکتنی_it;
	} وإلا إذا (ښکتنی_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		ښکتنی_it->دوهم.resize(range_دننه_واټن(ښکتنی_it, پای_ټکی_addr), State::Sx);
		// keep lower_it and upper_it
		ارزښتونه_2.erase(std::بل(ښکتنی_it), پاسنی_it);
		أرجع ښکتنی_it;
	} وإلا إذا (پاسنی_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const معلومات(State::Sx, (_range_begin(پاسنی_it) - begin_addr) * معلومات_پلنوالی_2);
		معلومات.append(std::move(پاسنی_it->دوهم));
		// delete lower_it and upper_it, then reinsert
		ارزښتونه_2.erase(ښکتنی_it, std::بل(پاسنی_it));
		أرجع ارزښتونه_2.emplace(begin_addr, std::move(معلومات)).لومړی;
	} وإلا {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		ارزښتونه_2.erase(ښکتنی_it, پاسنی_it);
		أرجع ارزښتونه_2.emplace(begin_addr, RTLIL::Const(State::Sx, (پای_ټکی_addr - begin_addr) * معلومات_پلنوالی_2)).لومړی;
	}
}

فراغ MemContents::ورننباسه_concatenated(addr_t addr, RTLIL::Const ثابت &ارزښتونه) {
	addr_t ټکي_2 = (ارزښتونه.اندازه() + معلومات_پلنوالی_2 - 1) / معلومات_پلنوالی_2;
	log_assert(addr < (addr_t)(1<<addr_پلنوالی_2));
	log_assert(ټکي_2 <= (addr_t)(1<<addr_پلنوالی_2) - addr);
	خپلکاری it = _reserve_range(addr, addr + ټکي_2);
	خپلکاری منزل_begin = range_معلومات(it, addr);
	std::copy(ارزښتونه.begin(), ارزښتونه.پای_ټکی(), منزل_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(منزل_begin + ارزښتونه.اندازه(), منزل_begin + ټکي_2 * معلومات_پلنوالی_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const ثابت &کلمه) {
	خپلکاری from_پای_ټکی = کلمه.اندازه() <= معلومات_پلنوالی_2 ? کلمه.پای_ټکی() : کلمه.begin() + معلومات_پلنوالی_2;
	خپلکاری منزل_پای_ټکی = std::copy(کلمه.begin(), from_پای_ټکی, it);
	خپلکاری it_بل = std::بل(it, معلومات_پلنوالی_2);
	std::fill(منزل_پای_ټکی, it_بل, State::S0);
	أرجع it_بل;
}
