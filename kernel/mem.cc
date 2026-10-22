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

#అన్తర్భావ "kernel/mem.h"
#అన్తర్భావ "kernel/ff.h"

USING_YOSYS_NAMESPACE

శూన్య Mem::remove() {
	ఒకవేళ (cell) {
		module->remove(cell);
		cell = శూన్యసూచిక;
	}
	ఒకవేళ (mem) {
		module->memories.erase(mem->name);
		తొలగించు mem;
		mem = శూన్యసూచిక;
	}
	కొరకు (స్వయంచాలక &పోర్టు : rd_ports) {
		ఒకవేళ (పోర్టు.cell) {
			module->remove(పోర్టు.cell);
			పోర్టు.cell = శూన్యసూచిక;
		}
	}
	కొరకు (స్వయంచాలక &పోర్టు : wr_ports) {
		ఒకవేళ (పోర్టు.cell) {
			module->remove(పోర్టు.cell);
			పోర్టు.cell = శూన్యసూచిక;
		}
	}
	కొరకు (స్వయంచాలక &init : inits) {
		ఒకవేళ (init.cell) {
			module->remove(init.cell);
			init.cell = శూన్యసూచిక;
		}
	}
}

శూన్య Mem::emit() {
	check();
	std::సదిశ<పూర్ణాంకం> rd_ఎడమ;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ports); i++) {
		స్వయంచాలక &పోర్టు = rd_ports[i];
		ఒకవేళ (పోర్టు.removed) {
			ఒకవేళ (పోర్టు.cell) {
				module->remove(పోర్టు.cell);
			}
		} లేకపోతే {
			rd_ఎడమ.నెట్టు_వెనుక(i);
		}
	}
	std::సదిశ<పూర్ణాంకం> wr_ఎడమ;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++) {
		స్వయంచాలక &పోర్టు = wr_ports[i];
		ఒకవేళ (పోర్టు.removed) {
			ఒకవేళ (పోర్టు.cell) {
				module->remove(పోర్టు.cell);
			}
		} లేకపోతే {
			wr_ఎడమ.నెట్టు_వెనుక(i);
		}
	}
	std::సదిశ<పూర్ణాంకం> init_ఎడమ;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(inits); i++) {
		స్వయంచాలక &init = inits[i];
		ఒకవేళ (init.removed) {
			ఒకవేళ (init.cell) {
				module->remove(init.cell);
			}
		} లేకపోతే {
			init_ఎడమ.నెట్టు_వెనుక(i);
		}
	}
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ఎడమ); i++)
		ఒకవేళ (i != rd_ఎడమ[i])
			std::మార్పిడి(rd_ports[i], rd_ports[rd_ఎడమ[i]]);
	rd_ports.resize(Get_కొలత(rd_ఎడమ));
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ఎడమ); i++)
		ఒకవేళ (i != wr_ఎడమ[i])
			std::మార్పిడి(wr_ports[i], wr_ports[wr_ఎడమ[i]]);
	wr_ports.resize(Get_కొలత(wr_ఎడమ));
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(init_ఎడమ); i++)
		ఒకవేళ (i != init_ఎడమ[i])
			std::మార్పిడి(inits[i], inits[init_ఎడమ[i]]);
	inits.resize(Get_కొలత(init_ఎడమ));

	కొరకు (స్వయంచాలక &పోర్టు : rd_ports) {
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ఎడమ); i++) {
			పోర్టు.transparency_ముసుగు[i] = పోర్టు.transparency_ముసుగు[wr_ఎడమ[i]];
			పోర్టు.collision_x_ముసుగు[i] = పోర్టు.collision_x_ముసుగు[wr_ఎడమ[i]];
		}
		పోర్టు.transparency_ముసుగు.resize(Get_కొలత(wr_ఎడమ));
		పోర్టు.collision_x_ముసుగు.resize(Get_కొలత(wr_ఎడమ));
	}
	కొరకు (స్వయంచాలక &పోర్టు : wr_ports) {
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ఎడమ); i++)
			పోర్టు.ప్రాధాన్యత_ముసుగు[i] = పోర్టు.ప్రాధాన్యత_ముసుగు[wr_ఎడమ[i]];
		పోర్టు.ప్రాధాన్యత_ముసుగు.resize(Get_కొలత(wr_ఎడమ));
	}

	ఒకవేళ (packed) {
		ఒకవేళ (mem) {
			module->memories.erase(mem->name);
			తొలగించు mem;
			mem = శూన్యసూచిక;
		}
		ఒకవేళ (!cell) {
			ఒకవేళ (memid.ఖాళీలు())
				memid = PooledName(module->design, module->twines().కూడు(NEW_ID));
			cell = module->కూడు_Cell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->లక్షణాలు = లక్షణాలు;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::వెడల్పు_2] = Const(వెడల్పు);
		cell->parameters[ID::స్థానభ్రంశం_2] = Const(ప్రారంభం_స్థానభ్రంశం);
		cell->parameters[ID::కొలత_2] = Const(కొలత);
		SigSpec rd_clk, rd_en, rd_addr, rd_దత్తాంశం;
		SigSpec wr_clk, wr_en, wr_addr, wr_దత్తాంశం;
		SigSpec rd_arst, rd_srst;
		పూర్ణాంకం abits = 0;
		కొరకు (స్వయంచాలక &పోర్టు : rd_ports)
			abits = std::max(abits, Get_కొలత(పోర్టు.addr));
		కొరకు (స్వయంచాలక &పోర్టు : wr_ports)
			abits = std::max(abits, Get_కొలత(పోర్టు.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::సదిశ<పూర్ణాంకం> wr_పోర్టు_xlat;
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++)
			కొరకు (పూర్ణాంకం j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_పోర్టు_xlat.నెట్టు_వెనుక(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_ముసుగు_builder;
		Const::Builder rd_collision_x_ముసుగు_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_విలువ_builder;
		Const::Builder rd_srst_విలువ_builder;
		Const::Builder rd_init_విలువ_builder;
		కొరకు (స్వయంచాలక &పోర్టు : rd_ports) {
			కొరకు (స్వయంచాలక attr: పోర్టు.లక్షణాలు)
				ఒకవేళ (!cell->has_attribute(attr.మొదటి))
					cell->లక్షణాలు.చొప్పించు(attr);
			ఒకవేళ (పోర్టు.cell) {
				module->remove(పోర్టు.cell);
				పోర్టు.cell = శూన్యసూచిక;
			}
			కొరకు (పూర్ణాంకం sub = 0; sub < (1 << పోర్టు.wide_log2); sub++)
			{
				rd_wide_continuation_builder.నెట్టు_వెనుక(State(sub != 0));
				rd_clk_enable_builder.నెట్టు_వెనుక(State(పోర్టు.clk_enable));
				rd_clk_polarity_builder.నెట్టు_వెనుక(State(పోర్టు.clk_polarity));
				rd_ce_over_srst_builder.నెట్టు_వెనుక(State(పోర్టు.ce_over_srst));
				rd_clk.append(పోర్టు.clk);
				rd_arst.append(పోర్టు.arst);
				rd_srst.append(పోర్టు.srst);
				rd_en.append(పోర్టు.en);
				SigSpec addr = పోర్టు.sub_addr(sub);
				addr.extend_u0(abits, అబద్ధం);
				rd_addr.append(addr);
				log_assert(Get_కొలత(addr) == abits);
				కొరకు (స్వయంచాలక idx : wr_పోర్టు_xlat) {
					rd_transparency_ముసుగు_builder.నెట్టు_వెనుక(State(తార్కిక(పోర్టు.transparency_ముసుగు[idx])));
					rd_collision_x_ముసుగు_builder.నెట్టు_వెనుక(State(తార్కిక(పోర్టు.collision_x_ముసుగు[idx])));
				}
			}
			rd_దత్తాంశం.append(పోర్టు.దత్తాంశం);
			కొరకు (స్వయంచాలక బిట్టు : పోర్టు.arst_విలువ)
				rd_arst_విలువ_builder.నెట్టు_వెనుక(బిట్టు);
			కొరకు (స్వయంచాలక బిట్టు : పోర్టు.srst_విలువ)
				rd_srst_విలువ_builder.నెట్టు_వెనుక(బిట్టు);
			కొరకు (స్వయంచాలక బిట్టు : పోర్టు.init_విలువ)
				rd_init_విలువ_builder.నెట్టు_వెనుక(బిట్టు);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_ముసుగు = rd_transparency_ముసుగు_builder.build();
		Const rd_collision_x_ముసుగు = rd_collision_x_ముసుగు_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_విలువ = rd_arst_విలువ_builder.build();
		Const rd_srst_విలువ = rd_srst_విలువ_builder.build();
		Const rd_init_విలువ = rd_init_విలువ_builder.build();
		ఒకవేళ (rd_ports.ఖాళీలు()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_విలువ = State::S0;
			rd_srst_విలువ = State::S0;
			rd_init_విలువ = State::S0;
		}
		ఒకవేళ (rd_ports.ఖాళీలు() || wr_ports.ఖాళీలు()) {
			rd_transparency_ముసుగు = State::S0;
			rd_collision_x_ముసుగు = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_కొలత(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_ముసుగు] = rd_transparency_ముసుగు;
		cell->parameters[ID::RD_COLLISION_X_ముసుగు] = rd_collision_x_ముసుగు;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_విలువ] = rd_arst_విలువ;
		cell->parameters[ID::RD_SRST_విలువ] = rd_srst_విలువ;
		cell->parameters[ID::RD_INIT_విలువ] = rd_init_విలువ;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->సమితి_పోర్టు(ID::RD_CLK, rd_clk);
		cell->సమితి_పోర్టు(ID::RD_EN, rd_en);
		cell->సమితి_పోర్టు(ID::RD_ARST, rd_arst);
		cell->సమితి_పోర్టు(ID::RD_SRST, rd_srst);
		cell->సమితి_పోర్టు(ID::RD_ADDR, rd_addr);
		cell->సమితి_పోర్టు(ID::RD_దత్తాంశం, rd_దత్తాంశం);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_ప్రాధాన్యత_ముసుగు_builder;
		కొరకు (స్వయంచాలక &పోర్టు : wr_ports) {
			కొరకు (స్వయంచాలక attr: పోర్టు.లక్షణాలు)
				ఒకవేళ (!cell->has_attribute(attr.మొదటి))
					cell->లక్షణాలు.చొప్పించు(attr);
			ఒకవేళ (పోర్టు.cell) {
				module->remove(పోర్టు.cell);
				పోర్టు.cell = శూన్యసూచిక;
			}
			కొరకు (పూర్ణాంకం sub = 0; sub < (1 << పోర్టు.wide_log2); sub++)
			{
				wr_wide_continuation_builder.నెట్టు_వెనుక(State(sub != 0));
				wr_clk_enable_builder.నెట్టు_వెనుక(State(పోర్టు.clk_enable));
				wr_clk_polarity_builder.నెట్టు_వెనుక(State(పోర్టు.clk_polarity));
				wr_clk.append(పోర్టు.clk);
				కొరకు (స్వయంచాలక idx : wr_పోర్టు_xlat)
					wr_ప్రాధాన్యత_ముసుగు_builder.నెట్టు_వెనుక(State(తార్కిక(పోర్టు.ప్రాధాన్యత_ముసుగు[idx])));
				SigSpec addr = పోర్టు.sub_addr(sub);
				addr.extend_u0(abits, అబద్ధం);
				wr_addr.append(addr);
				log_assert(Get_కొలత(addr) == abits);
			}
			wr_en.append(పోర్టు.en);
			wr_దత్తాంశం.append(పోర్టు.దత్తాంశం);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_ప్రాధాన్యత_ముసుగు = wr_ప్రాధాన్యత_ముసుగు_builder.build();
		ఒకవేళ (wr_ports.ఖాళీలు()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_ప్రాధాన్యత_ముసుగు = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_కొలత(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_ప్రాధాన్యత_ముసుగు] = wr_ప్రాధాన్యత_ముసుగు;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->సమితి_పోర్టు(ID::WR_CLK, wr_clk);
		cell->సమితి_పోర్టు(ID::WR_EN, wr_en);
		cell->సమితి_పోర్టు(ID::WR_ADDR, wr_addr);
		cell->సమితి_పోర్టు(ID::WR_దత్తాంశం, wr_దత్తాంశం);
		కొరకు (స్వయంచాలక &init : inits) {
			కొరకు (స్వయంచాలక attr: init.లక్షణాలు)
				ఒకవేళ (!cell->has_attribute(attr.మొదటి))
					cell->లక్షణాలు.చొప్పించు(attr);
			ఒకవేళ (init.cell) {
				module->remove(init.cell);
				init.cell = శూన్యసూచిక;
			}
		}
		cell->parameters[ID::INIT] = get_init_దత్తాంశం();
	} లేకపోతే {
		ఒకవేళ (cell) {
			module->remove(cell);
			cell = శూన్యసూచిక;
		}
		ఒకవేళ (!mem) {
			ఒకవేళ (memid.ఖాళీలు())
				memid = PooledName(module->design, module->twines().కూడు(NEW_ID));
			mem = module->కూడు_జ్ఞాపకం(memid);
		}
		mem->వెడల్పు = వెడల్పు;
		mem->ప్రారంభం_స్థానభ్రంశం = ప్రారంభం_స్థానభ్రంశం;
		mem->కొలత = కొలత;
		mem->లక్షణాలు = లక్షణాలు;
		కొరకు (స్వయంచాలక &పోర్టు : rd_ports) {
			ఒకవేళ (!పోర్టు.cell)
				పోర్టు.cell = module->కూడు_Cell(NEW_ID, ID($memrd_v2));
			పోర్టు.cell->type = ID($memrd_v2);
			పోర్టు.cell->లక్షణాలు = పోర్టు.లక్షణాలు;
			పోర్టు.cell->parameters[ID::MEMID] = memid.str();
			పోర్టు.cell->parameters[ID::ABITS] = Get_కొలత(పోర్టు.addr);
			పోర్టు.cell->parameters[ID::వెడల్పు_2] = వెడల్పు << పోర్టు.wide_log2;
			పోర్టు.cell->parameters[ID::CLK_ENABLE] = పోర్టు.clk_enable;
			పోర్టు.cell->parameters[ID::CLK_POLARITY] = పోర్టు.clk_polarity;
			పోర్టు.cell->parameters[ID::CE_OVER_SRST] = పోర్టు.ce_over_srst;
			పోర్టు.cell->parameters[ID::ARST_విలువ] = పోర్టు.arst_విలువ;
			పోర్టు.cell->parameters[ID::SRST_విలువ] = పోర్టు.srst_విలువ;
			పోర్టు.cell->parameters[ID::INIT_విలువ] = పోర్టు.init_విలువ;
			పోర్టు.cell->parameters[ID::TRANSPARENCY_ముసుగు] = పోర్టు.transparency_ముసుగు;
			పోర్టు.cell->parameters[ID::COLLISION_X_ముసుగు] = పోర్టు.collision_x_ముసుగు;
			పోర్టు.cell->parameters.erase(ID::TRANSPARENT);
			పోర్టు.cell->సమితి_పోర్టు(ID::CLK, పోర్టు.clk);
			పోర్టు.cell->సమితి_పోర్టు(ID::EN, పోర్టు.en);
			పోర్టు.cell->సమితి_పోర్టు(ID::ARST, పోర్టు.arst);
			పోర్టు.cell->సమితి_పోర్టు(ID::SRST, పోర్టు.srst);
			పోర్టు.cell->సమితి_పోర్టు(ID::ADDR, పోర్టు.addr);
			పోర్టు.cell->సమితి_పోర్టు(ID::దత్తాంశం_2, పోర్టు.దత్తాంశం);
		}
		పూర్ణాంకం idx = 0;
		కొరకు (స్వయంచాలక &పోర్టు : wr_ports) {
			ఒకవేళ (!పోర్టు.cell)
				పోర్టు.cell = module->కూడు_Cell(NEW_ID, ID($memwr_v2));
			పోర్టు.cell->type = ID($memwr_v2);
			పోర్టు.cell->లక్షణాలు = పోర్టు.లక్షణాలు;
			ఒకవేళ (పోర్టు.cell->parameters.లెక్క(ID::ప్రాధాన్యత))
				పోర్టు.cell->parameters.erase(ID::ప్రాధాన్యత);
			పోర్టు.cell->parameters[ID::MEMID] = memid.str();
			పోర్టు.cell->parameters[ID::ABITS] = Get_కొలత(పోర్టు.addr);
			పోర్టు.cell->parameters[ID::వెడల్పు_2] = వెడల్పు << పోర్టు.wide_log2;
			పోర్టు.cell->parameters[ID::CLK_ENABLE] = పోర్టు.clk_enable;
			పోర్టు.cell->parameters[ID::CLK_POLARITY] = పోర్టు.clk_polarity;
			పోర్టు.cell->parameters[ID::PORTID] = idx++;
			పోర్టు.cell->parameters[ID::ప్రాధాన్యత_ముసుగు_2] = పోర్టు.ప్రాధాన్యత_ముసుగు;
			పోర్టు.cell->సమితి_పోర్టు(ID::CLK, పోర్టు.clk);
			పోర్టు.cell->సమితి_పోర్టు(ID::EN, పోర్టు.en);
			పోర్టు.cell->సమితి_పోర్టు(ID::ADDR, పోర్టు.addr);
			పోర్టు.cell->సమితి_పోర్టు(ID::దత్తాంశం_2, పోర్టు.దత్తాంశం);
		}
		idx = 0;
		కొరకు (స్వయంచాలక &init : inits) {
			తార్కిక v2 = !init.en.is_fully_ones();
			ఒకవేళ (!init.cell)
				init.cell = module->కూడు_Cell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			లేకపోతే {
				ఒకవేళ (!v2)
					init.cell->unset_పోర్టు(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->లక్షణాలు = init.లక్షణాలు;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_కొలత(init.addr);
			init.cell->parameters[ID::వెడల్పు_2] = వెడల్పు;
			init.cell->parameters[ID::పదాలు] = Get_కొలత(init.దత్తాంశం) / వెడల్పు;
			init.cell->parameters[ID::ప్రాధాన్యత] = idx++;
			init.cell->సమితి_పోర్టు(ID::ADDR, init.addr);
			init.cell->సమితి_పోర్టు(ID::దత్తాంశం_2, init.దత్తాంశం);
			ఒకవేళ (v2)
				init.cell->సమితి_పోర్టు(ID::EN, init.en);
		}
	}
}

శూన్య Mem::clear_inits() {
	కొరకు (స్వయంచాలక &init : inits)
		init.removed = నిజం;
}

శూన్య Mem::coalesce_inits() {
	// start address -> end address
	std::map<పూర్ణాంకం, పూర్ణాంకం> chunks;
	// Figure out chunk boundaries.
	కొరకు (స్వయంచాలక &init : inits) {
		ఒకవేళ (init.removed)
			కొనసాగు;
		తార్కిక valid = అబద్ధం;
		కొరకు (స్వయంచాలక బిట్టు : init.en)
			ఒకవేళ (బిట్టు == State::S1)
				valid = నిజం;
		ఒకవేళ (!valid) {
			init.removed = నిజం;
			కొనసాగు;
		}
		పూర్ణాంకం addr = init.addr.as_int();
		పూర్ణాంకం addr_e = addr + Get_కొలత(init.దత్తాంశం) / వెడల్పు;
		స్వయంచాలక it_e = chunks.పై_bound(addr_e);
		స్వయంచాలక it = it_e;
		అయితే (it != chunks.begin()) {
			--it;
			ఒకవేళ (it->రెండవ < addr) {
				++it;
				ఆపు;
			}
		}
		ఒకవేళ (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} లేకపోతే {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			ఒకవేళ (it->మొదటి < addr)
				addr = it->మొదటి;
			స్వయంచాలక it_చివరి = it_e;
			it_చివరి--;
			ఒకవేళ (it_చివరి->రెండవ > addr_e)
				addr_e = it_చివరి->రెండవ;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<పూర్ణాంకం, std::సదిశ<పూర్ణాంకం>> inits_by_chunk;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(inits); i++) {
		స్వయంచాలక &init = inits[i];
		ఒకవేళ (init.removed)
			కొనసాగు;
		స్వయంచాలక it = chunks.పై_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->మొదటి].నెట్టు_వెనుక(i);
		పూర్ణాంకం addr = init.addr.as_int();
		పూర్ణాంకం addr_e = addr + Get_కొలత(init.దత్తాంశం) / వెడల్పు;
		log_assert(addr >= it->మొదటి && addr_e <= it->రెండవ);
	}
	// Process each chunk.
	కొరకు (స్వయంచాలక &it : inits_by_chunk) {
		పూర్ణాంకం caddr = it.మొదటి;
		పూర్ణాంకం caddr_e = chunks[caddr];
		స్వయంచాలక &chunk_inits = it.రెండవ;
		ఒకవేళ (Get_కొలత(chunk_inits) == 1) {
			స్వయంచాలక &init = inits[chunk_inits[0]];
			ఒకవేళ (!init.en.is_fully_ones()) {
				కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(init.దత్తాంశం); i++)
					ఒకవేళ (init.en[i % వెడల్పు] != State::S1)
						init.దత్తాంశం.సమితి(i, State::Sx);
				init.en = Const(State::S1, వెడల్పు);
			}
			కొనసాగు;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * వెడల్పు);
		కొరకు (పూర్ణాంకం idx : chunk_inits) {
			స్వయంచాలక &init = inits[idx];
			పూర్ణాంకం స్థానభ్రంశం = (init.addr.as_int() - caddr) * వెడల్పు;
			log_assert(స్థానభ్రంశం >= 0);
			log_assert(స్థానభ్రంశం + Get_కొలత(init.దత్తాంశం) <= Get_కొలత(cdata));
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(init.దత్తాంశం); i++)
				ఒకవేళ (init.en[i % వెడల్పు] == State::S1)
					cdata.సమితి(i+స్థానభ్రంశం, init.దత్తాంశం[i]);
			init.removed = నిజం;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.దత్తాంశం = cdata;
		new_init.en = Const(State::S1, వెడల్పు);
		inits.నెట్టు_వెనుక(new_init);
	}
}

Const Mem::get_init_దత్తాంశం() స్థిరరాశి {
	Const init_దత్తాంశం(State::Sx, వెడల్పు * కొలత);
	కొరకు (స్వయంచాలక &init : inits) {
		ఒకవేళ (init.removed)
			కొనసాగు;
		పూర్ణాంకం స్థానభ్రంశం = (init.addr.as_int() - ప్రారంభం_స్థానభ్రంశం) * వెడల్పు;
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(init.దత్తాంశం); i++)
			ఒకవేళ (0 <= i+స్థానభ్రంశం && i+స్థానభ్రంశం < Get_కొలత(init_దత్తాంశం) && init.en[i % వెడల్పు] == State::S1)
				init_దత్తాంశం.సమితి(i+స్థానభ్రంశం, init.దత్తాంశం[i]);
	}
	తిరిగి init_దత్తాంశం;
}

శూన్య Mem::check() {
	పూర్ణాంకం max_wide_log2 = 0;
	కొరకు (స్వయంచాలక &పోర్టు : rd_ports) {
		ఒకవేళ (పోర్టు.removed)
			కొనసాగు;
		log_assert(Get_కొలత(పోర్టు.clk) == 1);
		log_assert(Get_కొలత(పోర్టు.en) == 1);
		log_assert(Get_కొలత(పోర్టు.arst) == 1);
		log_assert(Get_కొలత(పోర్టు.srst) == 1);
		log_assert(Get_కొలత(పోర్టు.addr) >= పోర్టు.wide_log2);
		log_assert(Get_కొలత(పోర్టు.దత్తాంశం) == (వెడల్పు << పోర్టు.wide_log2));
		log_assert(Get_కొలత(పోర్టు.init_విలువ) == (వెడల్పు << పోర్టు.wide_log2));
		log_assert(Get_కొలత(పోర్టు.arst_విలువ) == (వెడల్పు << పోర్టు.wide_log2));
		log_assert(Get_కొలత(పోర్టు.srst_విలువ) == (వెడల్పు << పోర్టు.wide_log2));
		ఒకవేళ (!పోర్టు.clk_enable) {
			log_assert(పోర్టు.en == State::S1);
			log_assert(పోర్టు.arst == State::S0);
			log_assert(పోర్టు.srst == State::S0);
		}
		కొరకు (పూర్ణాంకం j = 0; j < పోర్టు.wide_log2; j++) {
			log_assert(పోర్టు.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, పోర్టు.wide_log2);
		log_assert(Get_కొలత(పోర్టు.transparency_ముసుగు) == Get_కొలత(wr_ports));
		log_assert(Get_కొలత(పోర్టు.collision_x_ముసుగు) == Get_కొలత(wr_ports));
		కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(wr_ports); j++) {
			స్వయంచాలక &wport = wr_ports[j];
			ఒకవేళ ((పోర్టు.transparency_ముసుగు[j] || పోర్టు.collision_x_ముసుగు[j]) && !wport.removed) {
				log_assert(పోర్టు.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(పోర్టు.clk == wport.clk);
				log_assert(పోర్టు.clk_polarity == wport.clk_polarity);
			}
			log_assert(!పోర్టు.transparency_ముసుగు[j] || !పోర్టు.collision_x_ముసుగు[j]);
		}
	}
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++) {
		స్వయంచాలక &పోర్టు = wr_ports[i];
		ఒకవేళ (పోర్టు.removed)
			కొనసాగు;
		log_assert(Get_కొలత(పోర్టు.clk) == 1);
		log_assert(Get_కొలత(పోర్టు.en) == (వెడల్పు << పోర్టు.wide_log2));
		log_assert(Get_కొలత(పోర్టు.దత్తాంశం) == (వెడల్పు << పోర్టు.wide_log2));
		log_assert(Get_కొలత(పోర్టు.addr) >= పోర్టు.wide_log2);
		కొరకు (పూర్ణాంకం j = 0; j < పోర్టు.wide_log2; j++) {
			log_assert(పోర్టు.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, పోర్టు.wide_log2);
		log_assert(Get_కొలత(పోర్టు.ప్రాధాన్యత_ముసుగు) == Get_కొలత(wr_ports));
		కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(wr_ports); j++) {
			స్వయంచాలక &wport = wr_ports[j];
			ఒకవేళ (పోర్టు.ప్రాధాన్యత_ముసుగు[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(పోర్టు.clk_enable == wport.clk_enable);
				ఒకవేళ (పోర్టు.clk_enable) {
					log_assert(పోర్టు.clk == wport.clk);
					log_assert(పోర్టు.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	పూర్ణాంకం ముసుగు = (1 << max_wide_log2) - 1;
	log_assert(!(ప్రారంభం_స్థానభ్రంశం & ముసుగు));
	log_assert(!(కొలత & ముసుగు));
	log_assert(వెడల్పు != 0);
}

నామాకాశ {

	నిర్మాణం Mem_సూచీ {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_సూచీ (Module *module) {
			కొరకు (స్వయంచాలక cell: module->cells()) {
				ఒకవేళ (cell->type.లోపల(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].చొప్పించు(cell);
				లేకపోతే ఒకవేళ (cell->type.లోపల(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].చొప్పించు(cell);
				లేకపోతే ఒకవేళ (cell->type.లోపల(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].చొప్పించు(cell);
			}
		}
	};

	Mem mem_from_జ్ఞాపకం(Module *module, RTLIL::జ్ఞాపకం *mem, స్థిరరాశి Mem_సూచీ &సూచీ) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->వెడల్పు, mem->ప్రారంభం_స్థానభ్రంశం, mem->కొలత);
		res.packed = అబద్ధం;
		res.mem = mem;
		res.లక్షణాలు = mem->లక్షణాలు;
		std::సదిశ<తార్కిక> rd_transparent;
		std::సదిశ<పూర్ణాంకం> wr_portid;
		ఒకవేళ (సూచీ.rd_ports.లెక్క(memid)) {
			కొరకు (స్వయంచాలక cell : సూచీ.rd_ports.at(memid)) {
				MemRd mrd;
				తార్కిక is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.లక్షణాలు = cell->లక్షణాలు;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->get_పోర్టు(ID::CLK);
				mrd.en = cell->get_పోర్టు(ID::EN);
				mrd.addr = cell->get_పోర్టు(ID::ADDR);
				mrd.దత్తాంశం = cell->get_పోర్టు(ID::దత్తాంశం_2);
				mrd.wide_log2 = ceil_log2(Get_కొలత(mrd.దత్తాంశం) / mem->వెడల్పు);
				తార్కిక transparent = అబద్ధం;
				ఒకవేళ (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = అబద్ధం;
					mrd.arst_విలువ = Const(State::Sx, mem->వెడల్పు << mrd.wide_log2);
					mrd.srst_విలువ = Const(State::Sx, mem->వెడల్పు << mrd.wide_log2);
					mrd.init_విలువ = Const(State::Sx, mem->వెడల్పు << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					ఒకవేళ (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = అబద్ధం;
						ఒకవేళ (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} లేకపోతే {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_విలువ = cell->parameters.at(ID::ARST_విలువ);
					mrd.srst_విలువ = cell->parameters.at(ID::SRST_విలువ);
					mrd.init_విలువ = cell->parameters.at(ID::INIT_విలువ);
					mrd.arst = cell->get_పోర్టు(ID::ARST);
					mrd.srst = cell->get_పోర్టు(ID::SRST);
				}
				res.rd_ports.నెట్టు_వెనుక(mrd);
				rd_transparent.నెట్టు_వెనుక(transparent);
			}
		}
		ఒకవేళ (సూచీ.wr_ports.లెక్క(memid)) {
			std::సదిశ<std::pair<పూర్ణాంకం, MemWr>> ports;
			కొరకు (స్వయంచాలక cell : సూచీ.wr_ports.at(memid)) {
				MemWr mwr;
				తార్కిక is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.లక్షణాలు = cell->లక్షణాలు;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->get_పోర్టు(ID::CLK);
				mwr.en = cell->get_పోర్టు(ID::EN);
				mwr.addr = cell->get_పోర్టు(ID::ADDR);
				mwr.దత్తాంశం = cell->get_పోర్టు(ID::దత్తాంశం_2);
				mwr.wide_log2 = ceil_log2(Get_కొలత(mwr.దత్తాంశం) / mem->వెడల్పు);
				ports.నెట్టు_వెనుక(std::make_pair(cell->parameters.at(is_compat ? ID::ప్రాధాన్యత : ID::PORTID).as_int(), mwr));
			}
			std::క్రమీకరించు(ports.begin(), ports.చివర(), [](స్థిరరాశి std::pair<పూర్ణాంకం, MemWr> &a, స్థిరరాశి std::pair<పూర్ణాంకం, MemWr> &b) { తిరిగి a.మొదటి < b.మొదటి; });
			కొరకు (స్వయంచాలక &it : ports) {
				res.wr_ports.నెట్టు_వెనుక(it.రెండవ);
				wr_portid.నెట్టు_వెనుక(it.మొదటి);
			}
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(res.wr_ports); i++) {
				స్వయంచాలక &పోర్టు = res.wr_ports[i];
				తార్కిక is_compat = పోర్టు.cell->type == ID($memwr);
				ఒకవేళ (is_compat) {
					పోర్టు.ప్రాధాన్యత_ముసుగు.resize(Get_కొలత(res.wr_ports));
					కొరకు (పూర్ణాంకం j = 0; j < i; j++) {
						స్వయంచాలక &oport = res.wr_ports[j];
						ఒకవేళ (పోర్టు.clk_enable != oport.clk_enable)
							కొనసాగు;
						ఒకవేళ (పోర్టు.clk_enable && పోర్టు.clk != oport.clk)
							కొనసాగు;
						ఒకవేళ (పోర్టు.clk_enable && పోర్టు.clk_polarity != oport.clk_polarity)
							కొనసాగు;
						పోర్టు.ప్రాధాన్యత_ముసుగు[j] = నిజం;
					}
				} లేకపోతే {
					Const orig_prio_ముసుగు = పోర్టు.cell->parameters.at(ID::ప్రాధాన్యత_ముసుగు_2);
					కొరకు (పూర్ణాంకం orig_portid : wr_portid) {
						తార్కిక has_prio = orig_portid < Get_కొలత(orig_prio_ముసుగు) && orig_prio_ముసుగు[orig_portid] == State::S1;
						పోర్టు.ప్రాధాన్యత_ముసుగు.నెట్టు_వెనుక(has_prio);
					}
				}
			}
		}
		ఒకవేళ (సూచీ.inits.లెక్క(memid)) {
			std::సదిశ<std::pair<పూర్ణాంకం, MemInit>> inits;
			కొరకు (స్వయంచాలక cell : సూచీ.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.లక్షణాలు = cell->లక్షణాలు;
				స్వయంచాలక addr = cell->get_పోర్టు(ID::ADDR);
				స్వయంచాలక దత్తాంశం = cell->get_పోర్టు(ID::దత్తాంశం_2);
				ఒకవేళ (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				ఒకవేళ (!దత్తాంశం.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(దత్తాంశం), cell);
				init.addr = addr.as_const();
				init.దత్తాంశం = దత్తాంశం.as_const();
				ఒకవేళ (cell->type == ID($meminit_v2)) {
					స్వయంచాలక en = cell->get_పోర్టు(ID::EN);
					ఒకవేళ (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} లేకపోతే {
					init.en = RTLIL::Const(State::S1, mem->వెడల్పు);
				}
				inits.నెట్టు_వెనుక(std::make_pair(cell->parameters.at(ID::ప్రాధాన్యత).as_int(), init));
			}
			std::క్రమీకరించు(inits.begin(), inits.చివర(), [](స్థిరరాశి std::pair<పూర్ణాంకం, MemInit> &a, స్థిరరాశి std::pair<పూర్ణాంకం, MemInit> &b) { తిరిగి a.మొదటి < b.మొదటి; });
			కొరకు (స్వయంచాలక &it : inits)
				res.inits.నెట్టు_వెనుక(it.రెండవ);
		}
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(res.rd_ports); i++) {
			స్వయంచాలక &పోర్టు = res.rd_ports[i];
			తార్కిక is_compat = పోర్టు.cell->type == ID($memrd);
			ఒకవేళ (is_compat) {
				పోర్టు.transparency_ముసుగు.resize(Get_కొలత(res.wr_ports));
				పోర్టు.collision_x_ముసుగు.resize(Get_కొలత(res.wr_ports));
				ఒకవేళ (!rd_transparent[i])
					కొనసాగు;
				ఒకవేళ (!పోర్టు.clk_enable)
					కొనసాగు;
				కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(res.wr_ports); j++) {
					స్వయంచాలక &wport = res.wr_ports[j];
					ఒకవేళ (!wport.clk_enable)
						కొనసాగు;
					ఒకవేళ (పోర్టు.clk != wport.clk)
						కొనసాగు;
					ఒకవేళ (పోర్టు.clk_polarity != wport.clk_polarity)
						కొనసాగు;
					పోర్టు.transparency_ముసుగు[j] = నిజం;
				}
			} లేకపోతే {
				Const orig_trans_ముసుగు = పోర్టు.cell->parameters.at(ID::TRANSPARENCY_ముసుగు);
				Const orig_cx_ముసుగు = పోర్టు.cell->parameters.at(ID::COLLISION_X_ముసుగు);
				కొరకు (పూర్ణాంకం orig_portid : wr_portid) {
					పోర్టు.transparency_ముసుగు.నెట్టు_వెనుక(orig_portid < Get_కొలత(orig_trans_ముసుగు) && orig_trans_ముసుగు[orig_portid] == State::S1);
					పోర్టు.collision_x_ముసుగు.నెట్టు_వెనుక(orig_portid < Get_కొలత(orig_cx_ముసుగు) && orig_cx_ముసుగు[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		తిరిగి res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().కూడు(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::వెడల్పు_2).as_int(),
			cell->parameters.at(ID::స్థానభ్రంశం_2).as_int(),
			cell->parameters.at(ID::కొలత_2).as_int()
		);
		తార్కిక is_compat = cell->type == ID($mem);
		పూర్ణాంకం abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = నిజం;
		res.cell = cell;
		res.లక్షణాలు = cell->లక్షణాలు;
		Const &init = cell->parameters.at(ID::INIT);
		ఒకవేళ (!init.is_fully_undef()) {
			పూర్ణాంకం pos = 0;
			అయితే (pos < res.కొలత) {
				Const మాట = init.వెలికితీయి(pos * res.వెడల్పు, res.వెడల్పు, State::Sx);
				ఒకవేళ (మాట.is_fully_undef()) {
					pos++;
				} లేకపోతే {
					పూర్ణాంకం epos;
					కొరకు (epos = pos; epos < res.కొలత; epos++) {
						Const eword = init.వెలికితీయి(epos * res.వెడల్పు, res.వెడల్పు, State::Sx);
						ఒకవేళ (eword.is_fully_undef())
							ఆపు;
					}
					MemInit minit;
					minit.addr = res.ప్రారంభం_స్థానభ్రంశం + pos;
					minit.దత్తాంశం = init.వెలికితీయి(pos * res.వెడల్పు, (epos - pos) * res.వెడల్పు, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.వెడల్పు);
					res.inits.నెట్టు_వెనుక(minit);
					pos = epos;
				}
			}
		}
		పూర్ణాంకం n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		పూర్ణాంకం n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		కొరకు (పూర్ణాంకం i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			అయితే (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).వెలికితీయి(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).వెలికితీయి(i, 1).as_bool();
			mrd.clk = cell->get_పోర్టు(ID::RD_CLK).వెలికితీయి(i, 1);
			mrd.en = cell->get_పోర్టు(ID::RD_EN).వెలికితీయి(i, 1);
			mrd.addr = cell->get_పోర్టు(ID::RD_ADDR).వెలికితీయి(i * abits, abits);
			mrd.దత్తాంశం = cell->get_పోర్టు(ID::RD_దత్తాంశం).వెలికితీయి(i * res.వెడల్పు, (ni - i) * res.వెడల్పు);
			ఒకవేళ (is_compat) {
				mrd.ce_over_srst = అబద్ధం;
				mrd.arst_విలువ = Const(State::Sx, res.వెడల్పు << mrd.wide_log2);
				mrd.srst_విలువ = Const(State::Sx, res.వెడల్పు << mrd.wide_log2);
				mrd.init_విలువ = Const(State::Sx, res.వెడల్పు << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} లేకపోతే {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).వెలికితీయి(i, 1).as_bool();
				mrd.arst_విలువ = cell->parameters.at(ID::RD_ARST_విలువ).వెలికితీయి(i * res.వెడల్పు, (ni - i) * res.వెడల్పు);
				mrd.srst_విలువ = cell->parameters.at(ID::RD_SRST_విలువ).వెలికితీయి(i * res.వెడల్పు, (ni - i) * res.వెడల్పు);
				mrd.init_విలువ = cell->parameters.at(ID::RD_INIT_విలువ).వెలికితీయి(i * res.వెడల్పు, (ni - i) * res.వెడల్పు);
				mrd.arst = cell->get_పోర్టు(ID::RD_ARST).వెలికితీయి(i, 1);
				mrd.srst = cell->get_పోర్టు(ID::RD_SRST).వెలికితీయి(i, 1);
			}
			ఒకవేళ (!is_compat) {
				Const transparency_ముసుగు = cell->parameters.at(ID::RD_TRANSPARENCY_ముసుగు).వెలికితీయి(i * n_wr_ports, n_wr_ports);
				Const collision_x_ముసుగు = cell->parameters.at(ID::RD_COLLISION_X_ముసుగు).వెలికితీయి(i * n_wr_ports, n_wr_ports);
				కొరకు (పూర్ణాంకం j = 0; j < n_wr_ports; j++)
					ఒకవేళ (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_ముసుగు.నెట్టు_వెనుక(transparency_ముసుగు[j] == State::S1);
						mrd.collision_x_ముసుగు.నెట్టు_వెనుక(collision_x_ముసుగు[j] == State::S1);
					}
			}
			res.rd_ports.నెట్టు_వెనుక(mrd);
		}
		కొరకు (పూర్ణాంకం i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			అయితే (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).వెలికితీయి(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).వెలికితీయి(i, 1).as_bool();
			mwr.clk = cell->get_పోర్టు(ID::WR_CLK).వెలికితీయి(i, 1);
			mwr.en = cell->get_పోర్టు(ID::WR_EN).వెలికితీయి(i * res.వెడల్పు, (ni - i) * res.వెడల్పు);
			mwr.addr = cell->get_పోర్టు(ID::WR_ADDR).వెలికితీయి(i * abits, abits);
			mwr.దత్తాంశం = cell->get_పోర్టు(ID::WR_దత్తాంశం).వెలికితీయి(i * res.వెడల్పు, (ni - i) * res.వెడల్పు);
			ఒకవేళ (!is_compat) {
				Const ప్రాధాన్యత_ముసుగు = cell->parameters.at(ID::WR_ప్రాధాన్యత_ముసుగు).వెలికితీయి(i * n_wr_ports, n_wr_ports);
				కొరకు (పూర్ణాంకం j = 0; j < n_wr_ports; j++)
					ఒకవేళ (wr_wide_continuation[j] != State::S1)
						mwr.ప్రాధాన్యత_ముసుగు.నెట్టు_వెనుక(ప్రాధాన్యత_ముసుగు[j] == State::S1);
			}
			res.wr_ports.నెట్టు_వెనుక(mwr);
		}
		ఒకవేళ (is_compat) {
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(res.wr_ports); i++) {
				స్వయంచాలక &పోర్టు = res.wr_ports[i];
				పోర్టు.ప్రాధాన్యత_ముసుగు.resize(Get_కొలత(res.wr_ports));
				కొరకు (పూర్ణాంకం j = 0; j < i; j++) {
					స్వయంచాలక &oport = res.wr_ports[j];
					ఒకవేళ (పోర్టు.clk_enable != oport.clk_enable)
						కొనసాగు;
					ఒకవేళ (పోర్టు.clk_enable && పోర్టు.clk != oport.clk)
						కొనసాగు;
					ఒకవేళ (పోర్టు.clk_enable && పోర్టు.clk_polarity != oport.clk_polarity)
						కొనసాగు;
					పోర్టు.ప్రాధాన్యత_ముసుగు[j] = నిజం;
				}
			}
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(res.rd_ports); i++) {
				స్వయంచాలక &పోర్టు = res.rd_ports[i];
				పోర్టు.transparency_ముసుగు.resize(Get_కొలత(res.wr_ports));
				పోర్టు.collision_x_ముసుగు.resize(Get_కొలత(res.wr_ports));
				ఒకవేళ (!cell->parameters.at(ID::RD_TRANSPARENT).వెలికితీయి(i, 1).as_bool())
					కొనసాగు;
				ఒకవేళ (!పోర్టు.clk_enable)
					కొనసాగు;
				కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(res.wr_ports); j++) {
					స్వయంచాలక &wport = res.wr_ports[j];
					ఒకవేళ (!wport.clk_enable)
						కొనసాగు;
					ఒకవేళ (పోర్టు.clk != wport.clk)
						కొనసాగు;
					ఒకవేళ (పోర్టు.clk_polarity != wport.clk_polarity)
						కొనసాగు;
					పోర్టు.transparency_ముసుగు[j] = నిజం;
				}
			}
		}
		res.check();
		తిరిగి res;
	}

}

std::సదిశ<Mem> Mem::get_all_memories(Module *module) {
	std::సదిశ<Mem> res;
	Mem_సూచీ సూచీ(module);
	కొరకు (స్వయంచాలక it: module->memories) {
		res.నెట్టు_వెనుక(mem_from_జ్ఞాపకం(module, it.రెండవ, సూచీ));
	}
	కొరకు (స్వయంచాలక cell: module->cells()) {
		ఒకవేళ (cell->type.లోపల(ID($mem), ID($mem_v2)))
			res.నెట్టు_వెనుక(mem_from_cell(cell));
	}
	తిరిగి res;
}

std::సదిశ<Mem> Mem::get_selected_memories(Module *module) {
	std::సదిశ<Mem> res;
	Mem_సూచీ సూచీ(module);
	కొరకు (స్వయంచాలక it: module->memories) {
		ఒకవేళ (module->design->selected(module, it.రెండవ))
			res.నెట్టు_వెనుక(mem_from_జ్ఞాపకం(module, it.రెండవ, సూచీ));
	}
	కొరకు (స్వయంచాలక cell: module->selected_cells()) {
		ఒకవేళ (cell->type.లోపల(ID($mem), ID($mem_v2)))
			res.నెట్టు_వెనుక(mem_from_cell(cell));
	}
	తిరిగి res;
}

Cell *Mem::వెలికితీయి_rdff(పూర్ణాంకం idx, FfInitVals *initvals) {
	MemRd &పోర్టు = rd_ports[idx];

	ఒకవేళ (!పోర్టు.clk_enable)
		తిరిగి శూన్యసూచిక;

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
	తార్కిక trans_use_addr = నిజం;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++)
		ఒకవేళ (!పోర్టు.transparency_ముసుగు[i] && !wr_ports[i].removed)
			trans_use_addr = అబద్ధం;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	ఒకవేళ (Get_కొలత(wr_ports) == 0)
		trans_use_addr = అబద్ధం;

	ఒకవేళ (పోర్టు.en != State::S1 || పోర్టు.srst != State::S0 || పోర్టు.arst != State::S0 || !పోర్టు.init_విలువ.is_fully_undef())
		trans_use_addr = అబద్ధం;

	ఒకవేళ (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		పూర్ణాంకం వెడల్పు = 0;
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(పోర్టు.addr); i++)
			ఒకవేళ (పోర్టు.addr[i].wire)
				వెడల్పు++;

		ఒకవేళ (వెడల్పు)
		{
			SigSpec sig_q = module->కూడు_Wire(stringf("$%s$rdreg[%d]$q", memid_str, idx), వెడల్పు);
			SigSpec sig_d;

			పూర్ణాంకం pos = 0;
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(పోర్టు.addr); i++)
				ఒకవేళ (పోర్టు.addr[i].wire) {
					sig_d.append(పోర్టు.addr[i]);
					పోర్టు.addr[i] = sig_q[pos++];
				}

			c = module->కూడు_Dff(stringf("$%s$rdreg[%d]", memid_str, idx), పోర్టు.clk, sig_d, sig_q, పోర్టు.clk_polarity);
		} లేకపోతే {
			c = శూన్యసూచిక;
		}
	}
	లేకపోతే
	{
		log_assert(పోర్టు.arst == State::S0 || పోర్టు.srst == State::S0);

		SigSpec async_d = module->కూడు_Wire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_కొలత(పోర్టు.దత్తాంశం));
		SigSpec sig_d = async_d;

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++) {
			స్వయంచాలక &wport = wr_ports[i];
			ఒకవేళ (wport.removed)
				కొనసాగు;
			ఒకవేళ (పోర్టు.transparency_ముసుగు[i] || పోర్టు.collision_x_ముసుగు[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == పోర్టు.clk);
				log_assert(wport.clk_enable == పోర్టు.clk_enable);
				పూర్ణాంకం min_wide_log2 = std::min(పోర్టు.wide_log2, wport.wide_log2);
				పూర్ణాంకం max_wide_log2 = std::max(పోర్టు.wide_log2, wport.wide_log2);
				తార్కిక wide_write = wport.wide_log2 > పోర్టు.wide_log2;
				కొరకు (పూర్ణాంకం sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = పోర్టు.addr;
					SigSpec waddr = wport.addr;
					ఒకవేళ (wide_write)
						waddr = wport.sub_addr(sub);
					లేకపోతే
						raddr = పోర్టు.sub_addr(sub);
					SigSpec addr_eq;
					ఒకవేళ (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					పూర్ణాంకం pos = 0;
					పూర్ణాంకం ewidth = వెడల్పు << min_wide_log2;
					పూర్ణాంకం wsub = wide_write ? sub : 0;
					పూర్ణాంకం rsub = wide_write ? 0 : sub;
					అయితే (pos < ewidth) {
						పూర్ణాంకం epos = pos;
						అయితే (epos < ewidth && wport.en[epos + wsub * వెడల్పు] == wport.en[pos + wsub * వెడల్పు])
							epos++;
						SigSpec cur = sig_d.వెలికితీయి(pos + rsub * వెడల్పు, epos-pos);
						SigSpec other = పోర్టు.transparency_ముసుగు[i] ? wport.దత్తాంశం.వెలికితీయి(pos + wsub * వెడల్పు, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						ఒకవేళ (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * వెడల్పు], addr_eq);
						లేకపోతే
							cond = wport.en[pos + wsub * వెడల్పు];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * వెడల్పు, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().కూడు(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_దత్తాంశం ff(module, initvals, name);
		ff.వెడల్పు = Get_కొలత(పోర్టు.దత్తాంశం);
		ff.has_clk = నిజం;
		ff.sig_clk = పోర్టు.clk;
		ff.pol_clk = పోర్టు.clk_polarity;
		ఒకవేళ (పోర్టు.en != State::S1) {
			ff.has_ce = నిజం;
			ff.pol_ce = నిజం;
			ff.sig_ce = పోర్టు.en;
		}
		ఒకవేళ (పోర్టు.arst != State::S0) {
			ff.has_arst = నిజం;
			ff.pol_arst = నిజం;
			ff.sig_arst = పోర్టు.arst;
			ff.val_arst = పోర్టు.arst_విలువ;
		}
		ఒకవేళ (పోర్టు.srst != State::S0) {
			ff.has_srst = నిజం;
			ff.pol_srst = నిజం;
			ff.sig_srst = పోర్టు.srst;
			ff.val_srst = పోర్టు.srst_విలువ;
			ff.ce_over_srst = ff.has_ce && పోర్టు.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = పోర్టు.దత్తాంశం;
		ff.val_init = పోర్టు.init_విలువ;
		పోర్టు.దత్తాంశం = async_d;
		c = ff.emit();
	}

	ఒకవేళ (c)
		సంవర్గమానం("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	పోర్టు.en = State::S1;
	పోర్టు.clk = State::S0;
	పోర్టు.arst = State::S0;
	పోర్టు.srst = State::S0;
	పోర్టు.clk_enable = అబద్ధం;
	పోర్టు.clk_polarity = నిజం;
	పోర్టు.ce_over_srst = అబద్ధం;
	పోర్టు.arst_విలువ = Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం));
	పోర్టు.srst_విలువ = Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం));
	పోర్టు.init_విలువ = Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం));

	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++) {
		పోర్టు.transparency_ముసుగు[i] = అబద్ధం;
		పోర్టు.collision_x_ముసుగు[i] = అబద్ధం;
	}

	తిరిగి c;
}

శూన్య Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::సదిశ<MemRd> new_rd_ports;
	std::సదిశ<MemWr> new_wr_ports;
	std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>> new_rd_map;
	std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>> new_wr_map;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ports); i++) {
		స్వయంచాలక &పోర్టు = rd_ports[i];
		కొరకు (పూర్ణాంకం sub = 0; sub < (1 << పోర్టు.wide_log2); sub++) {
			new_rd_map.నెట్టు_వెనుక(std::make_pair(i, sub));
		}
	}
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++) {
		స్వయంచాలక &పోర్టు = wr_ports[i];
		కొరకు (పూర్ణాంకం sub = 0; sub < (1 << పోర్టు.wide_log2); sub++) {
			new_wr_map.నెట్టు_వెనుక(std::make_pair(i, sub));
		}
	}
	కొరకు (స్వయంచాలక &it : new_rd_map) {
		MemRd &orig = rd_ports[it.మొదటి];
		MemRd పోర్టు = orig;
		ఒకవేళ (it.రెండవ != 0)
			పోర్టు.cell = శూన్యసూచిక;
		ఒకవేళ (పోర్టు.wide_log2) {
			పోర్టు.దత్తాంశం = పోర్టు.దత్తాంశం.వెలికితీయి(it.రెండవ * వెడల్పు, వెడల్పు);
			పోర్టు.init_విలువ = పోర్టు.init_విలువ.వెలికితీయి(it.రెండవ * వెడల్పు, వెడల్పు);
			పోర్టు.arst_విలువ = పోర్టు.arst_విలువ.వెలికితీయి(it.రెండవ * వెడల్పు, వెడల్పు);
			పోర్టు.srst_విలువ = పోర్టు.srst_విలువ.వెలికితీయి(it.రెండవ * వెడల్పు, వెడల్పు);
			పోర్టు.addr = పోర్టు.sub_addr(it.రెండవ);
			పోర్టు.wide_log2 = 0;
		}
		పోర్టు.transparency_ముసుగు.clear();
		పోర్టు.collision_x_ముసుగు.clear();
		కొరకు (స్వయంచాలక &it2 : new_wr_map)
			పోర్టు.transparency_ముసుగు.నెట్టు_వెనుక(orig.transparency_ముసుగు[it2.మొదటి]);
		కొరకు (స్వయంచాలక &it2 : new_wr_map)
			పోర్టు.collision_x_ముసుగు.నెట్టు_వెనుక(orig.collision_x_ముసుగు[it2.మొదటి]);
		new_rd_ports.నెట్టు_వెనుక(పోర్టు);
	}
	కొరకు (స్వయంచాలక &it : new_wr_map) {
		MemWr &orig = wr_ports[it.మొదటి];
		MemWr పోర్టు = orig;
		ఒకవేళ (it.రెండవ != 0)
			పోర్టు.cell = శూన్యసూచిక;
		ఒకవేళ (పోర్టు.wide_log2) {
			పోర్టు.దత్తాంశం = పోర్టు.దత్తాంశం.వెలికితీయి(it.రెండవ * వెడల్పు, వెడల్పు);
			పోర్టు.en = పోర్టు.en.వెలికితీయి(it.రెండవ * వెడల్పు, వెడల్పు);
			పోర్టు.addr = పోర్టు.sub_addr(it.రెండవ);
			పోర్టు.wide_log2 = 0;
		}
		పోర్టు.ప్రాధాన్యత_ముసుగు.clear();
		కొరకు (స్వయంచాలక &it2 : new_wr_map)
			పోర్టు.ప్రాధాన్యత_ముసుగు.నెట్టు_వెనుక(orig.ప్రాధాన్యత_ముసుగు[it2.మొదటి]);
		new_wr_ports.నెట్టు_వెనుక(పోర్టు);
	}
	std::మార్పిడి(rd_ports, new_rd_ports);
	std::మార్పిడి(wr_ports, new_wr_ports);
}

శూన్య Mem::emulate_ప్రాధాన్యత(పూర్ణాంకం idx1, పూర్ణాంకం idx2, FfInitVals *initvals)
{
	స్వయంచాలక &port1 = wr_ports[idx1];
	స్వయంచాలక &port2 = wr_ports[idx2];
	ఒకవేళ (!port2.ప్రాధాన్యత_ముసుగు[idx1])
		తిరిగి;
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ports); i++) {
		స్వయంచాలక &rport = rd_ports[i];
		ఒకవేళ (rport.removed)
			కొనసాగు;
		ఒకవేళ (rport.transparency_ముసుగు[idx1] && !(rport.transparency_ముసుగు[idx2] || rport.collision_x_ముసుగు[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	పూర్ణాంకం min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	పూర్ణాంకం max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	తార్కిక wide1 = port1.wide_log2 > port2.wide_log2;
	కొరకు (పూర్ణాంకం sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		ఒకవేళ (wide1)
			addr1 = port1.sub_addr(sub);
		లేకపోతే
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		పూర్ణాంకం ewidth = వెడల్పు << min_wide_log2;
		పూర్ణాంకం sub1 = wide1 ? sub : 0;
		పూర్ణాంకం sub2 = wide1 ? 0 : sub;
		dict<std::pair<Sig_బిట్టు, Sig_బిట్టు>, Sig_బిట్టు> cache;
		కొరకు (పూర్ణాంకం pos = 0; pos < ewidth; pos++) {
			Sig_బిట్టు &en1 = port1.en[pos + sub1 * వెడల్పు];
			Sig_బిట్టు &en2 = port2.en[pos + sub2 * వెడల్పు];
			std::pair<Sig_బిట్టు, Sig_బిట్టు> కీలకం(en1, en2);
			ఒకవేళ (cache.లెక్క(కీలకం)) {
				en1 = cache[కీలకం];
			} లేకపోతే {
				Sig_బిట్టు active2 = module->And(NEW_ID, addr_eq, en2);
				Sig_బిట్టు nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[కీలకం] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.ప్రాధాన్యత_ముసుగు[idx1] = అబద్ధం;
}

శూన్య Mem::emulate_transparency(పూర్ణాంకం widx, పూర్ణాంకం ridx, FfInitVals *initvals) {
	స్వయంచాలక &wport = wr_ports[widx];
	స్వయంచాలక &rport = rd_ports[ridx];
	log_assert(rport.transparency_ముసుగు[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	కొరకు (పూర్ణాంకం i = Get_కొలత(wr_ports) - 1; i > widx; i--) {
		ఒకవేళ (wr_ports[i].removed)
			కొనసాగు;
		ఒకవేళ (rport.transparency_ముసుగు[i] && wr_ports[i].ప్రాధాన్యత_ముసుగు[widx])
			emulate_transparency(i, ridx, initvals);
	}
	పూర్ణాంకం min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	పూర్ణాంకం max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	తార్కిక wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->కూడు_Wire(NEW_ID, Get_కొలత(wport.దత్తాంశం));
	module->కూడు_Dffe(NEW_ID, rport.clk, rport.en, wport.దత్తాంశం, wdata_q, rport.clk_polarity, నిజం);
	కొరకు (పూర్ణాంకం sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		కొరకు (పూర్ణాంకం j = min_wide_log2; j < max_wide_log2; j++)
			ఒకవేళ (wide_write)
				waddr = wport.sub_addr(sub);
			లేకపోతే
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		ఒకవేళ (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		పూర్ణాంకం pos = 0;
		పూర్ణాంకం ewidth = వెడల్పు << min_wide_log2;
		పూర్ణాంకం wsub = wide_write ? sub : 0;
		పూర్ణాంకం rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->కూడు_Wire(NEW_ID, ewidth);
		అయితే (pos < ewidth) {
			పూర్ణాంకం epos = pos;
			అయితే (epos < ewidth && wport.en[epos + wsub * వెడల్పు] == wport.en[pos + wsub * వెడల్పు])
				epos++;
			SigSpec cond;
			ఒకవేళ (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * వెడల్పు], addr_eq);
			లేకపోతే
				cond = wport.en[pos + wsub * వెడల్పు];
			SigSpec cond_q = module->కూడు_Wire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_దత్తాంశం ff(module, initvals, NEW_ID);
			ff.వెడల్పు = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = నిజం;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			ఒకవేళ (rport.en != State::S1) {
				ff.has_ce = నిజం;
				ff.sig_ce = rport.en;
				ff.pol_ce = నిజం;
			}
			ఒకవేళ (rport.arst != State::S0) {
				ff.has_arst = నిజం;
				ff.sig_arst = rport.arst;
				ff.pol_arst = నిజం;
				ff.val_arst = State::S0;
			}
			ఒకవేళ (rport.srst != State::S0) {
				ff.has_srst = నిజం;
				ff.sig_srst = rport.srst;
				ff.pol_srst = నిజం;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			ఒకవేళ (!rport.init_విలువ.is_fully_undef())
				ff.val_init = State::S0;
			లేకపోతే
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.వెలికితీయి(pos, epos-pos);
			SigSpec other = wdata_q.వెలికితీయి(pos + wsub * వెడల్పు, epos-pos);
			SigSpec dest = rport.దత్తాంశం.వెలికితీయి(pos + rsub * వెడల్పు, epos-pos);
			module->కూడు_Mux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.దత్తాంశం.replace(rsub * వెడల్పు, rdata_a);
	}
	rport.transparency_ముసుగు[widx] = అబద్ధం;
	rport.collision_x_ముసుగు[widx] = నిజం;
}

శూన్య Mem::prepare_wr_విలీనం(పూర్ణాంకం idx1, పూర్ణాంకం idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	స్వయంచాలక &port1 = wr_ports[idx1];
	స్వయంచాలక &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	కొరకు (పూర్ణాంకం i = 0; i < idx1; i++)
		ఒకవేళ (port2.ప్రాధాన్యత_ముసుగు[i])
			port1.ప్రాధాన్యత_ముసుగు[i] = నిజం;
	// If port 2 has priority over a port after port 1, emulate it.
	కొరకు (పూర్ణాంకం i = idx1 + 1; i < idx2; i++)
		ఒకవేళ (port2.ప్రాధాన్యత_ముసుగు[i] && !wr_ports[i].removed)
			emulate_ప్రాధాన్యత(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	కొరకు (పూర్ణాంకం i = idx2 + 1; i < Get_కొలత(wr_ports); i++) {
		స్వయంచాలక &oport = wr_ports[i];
		ఒకవేళ (oport.ప్రాధాన్యత_ముసుగు[idx2])
			oport.ప్రాధాన్యత_ముసుగు[idx1] = నిజం;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ports); i++) {
		స్వయంచాలక &rport = rd_ports[i];
		ఒకవేళ (rport.removed)
			కొనసాగు;
		// If collision already undefined with both ports, it's fine.
		ఒకవేళ (rport.collision_x_ముసుగు[idx1] && rport.collision_x_ముసుగు[idx2])
			కొనసాగు;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		ఒకవేళ (rport.collision_x_ముసుగు[idx1]) {
			rport.collision_x_ముసుగు[idx1] = అబద్ధం;
			rport.transparency_ముసుగు[idx1] = rport.transparency_ముసుగు[idx2];
			కొనసాగు;
		}
		ఒకవేళ (rport.collision_x_ముసుగు[idx2]) {
			rport.collision_x_ముసుగు[idx2] = అబద్ధం;
			rport.transparency_ముసుగు[idx2] = rport.transparency_ముసుగు[idx1];
			కొనసాగు;
		}
		// If transparent with both ports, also fine.
		ఒకవేళ (rport.transparency_ముసుగు[idx1] && rport.transparency_ముసుగు[idx2])
			కొనసాగు;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		ఒకవేళ (rport.transparency_ముసుగు[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_ముసుగు[idx1] = అబద్ధం;
			కొనసాగు;
		}
		ఒకవేళ (rport.transparency_ముసుగు[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_ముసుగు[idx2] = అబద్ధం;
			కొనసాగు;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

శూన్య Mem::prepare_rd_విలీనం(పూర్ణాంకం idx1, పూర్ణాంకం idx2, FfInitVals *initvals) {
	స్వయంచాలక &port1 = rd_ports[idx1];
	స్వయంచాలక &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wr_ports); i++) {
		ఒకవేళ (wr_ports[i].removed)
			కొనసాగు;
		// Both ports undefined, OK.
		ఒకవేళ (port1.collision_x_ముసుగు[i] && port2.collision_x_ముసుగు[i])
			కొనసాగు;
		// Only one port undefined — change its behavior
		// to align with the other port.
		ఒకవేళ (port1.collision_x_ముసుగు[i]) {
			port1.collision_x_ముసుగు[i] = అబద్ధం;
			port1.transparency_ముసుగు[i] = port2.transparency_ముసుగు[i];
			కొనసాగు;
		}
		ఒకవేళ (port2.collision_x_ముసుగు[i]) {
			port2.collision_x_ముసుగు[i] = అబద్ధం;
			port2.transparency_ముసుగు[i] = port1.transparency_ముసుగు[i];
			కొనసాగు;
		}
		// Both ports transparent, OK.
		ఒకవేళ (port1.transparency_ముసుగు[i] && port2.transparency_ముసుగు[i])
			కొనసాగు;
		// Only one port transparent — emulate transparency
		// on the other.
		ఒకవేళ (port1.transparency_ముసుగు[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_ముసుగు[i] = అబద్ధం;
			కొనసాగు;
		}
		ఒకవేళ (port2.transparency_ముసుగు[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_ముసుగు[i] = అబద్ధం;
			కొనసాగు;
		}
		// No ports transparent, OK.
	}

}

శూన్య Mem::widen_prep(పూర్ణాంకం wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	పూర్ణాంకం ముసుగు = ((1 << wide_log2) - 1);
	పూర్ణాంకం delta = ప్రారంభం_స్థానభ్రంశం & ముసుగు;
	ప్రారంభం_స్థానభ్రంశం -= delta;
	కొలత += delta;
	ఒకవేళ (కొలత & ముసుగు) {
		కొలత |= ముసుగు;
		కొలత++;
	}
}

శూన్య Mem::widen_wr_పోర్టు(పూర్ణాంకం idx, పూర్ణాంకం wide_log2) {
	widen_prep(wide_log2);
	స్వయంచాలక &పోర్టు = wr_ports[idx];
	log_assert(పోర్టు.wide_log2 <= wide_log2);
	ఒకవేళ (పోర్టు.wide_log2 < wide_log2) {
		SigSpec new_దత్తాంశం, new_en;
		SigSpec addr_lo = పోర్టు.addr.వెలికితీయి(0, wide_log2);
		కొరకు (పూర్ణాంకం sub = 0; sub < (1 << wide_log2); sub += (1 << పోర్టు.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			ఒకవేళ (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_దత్తాంశం.append(పోర్టు.దత్తాంశం);
				new_en.append(పోర్టు.en);
			} లేకపోతే ఒకవేళ (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_దత్తాంశం.append(Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం)));
				new_en.append(Const(State::S0, Get_కొలత(పోర్టు.దత్తాంశం)));
			} లేకపోతే {
				// May or may not write to this subword.
				new_దత్తాంశం.append(పోర్టు.దత్తాంశం);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_కొలత(పోర్టు.దత్తాంశం)), పోర్టు.en, addr_eq);
				new_en.append(en);
			}
		}
		పోర్టు.addr.replace(పోర్టు.wide_log2, Const(State::S0, wide_log2 - పోర్టు.wide_log2));
		పోర్టు.దత్తాంశం = new_దత్తాంశం;
		పోర్టు.en = new_en;
		పోర్టు.wide_log2 = wide_log2;
	}
}

శూన్య Mem::emulate_rden(పూర్ణాంకం idx, FfInitVals *initvals) {
	స్వయంచాలక &పోర్టు = rd_ports[idx];
	log_assert(పోర్టు.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_దత్తాంశం = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.దత్తాంశం));
	Wire *prev_దత్తాంశం = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.దత్తాంశం));
	Wire *sel = module->కూడు_Wire(NEW_ID);
	Ff_దత్తాంశం ff_sel(module, initvals, NEW_ID);
	Ff_దత్తాంశం ff_దత్తాంశం(module, initvals, NEW_ID);
	ff_sel.వెడల్పు = 1;
	ff_sel.has_clk = నిజం;
	ff_sel.sig_clk = పోర్టు.clk;
	ff_sel.pol_clk = పోర్టు.clk_polarity;
	ff_sel.sig_d = పోర్టు.en;
	ff_sel.sig_q = sel;
	ff_దత్తాంశం.వెడల్పు = Get_కొలత(పోర్టు.దత్తాంశం);
	ff_దత్తాంశం.has_clk = నిజం;
	ff_దత్తాంశం.sig_clk = పోర్టు.clk;
	ff_దత్తాంశం.pol_clk = పోర్టు.clk_polarity;
	ff_దత్తాంశం.sig_d = పోర్టు.దత్తాంశం;
	ff_దత్తాంశం.sig_q = prev_దత్తాంశం;
	ఒకవేళ (!పోర్టు.init_విలువ.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_దత్తాంశం.val_init = పోర్టు.init_విలువ;
		పోర్టు.init_విలువ = Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం));
	} లేకపోతే {
		ff_sel.val_init = State::Sx;
		ff_దత్తాంశం.val_init = Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం));
	}
	ఒకవేళ (పోర్టు.arst != State::S0) {
		ff_sel.has_arst = నిజం;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = పోర్టు.arst;
		ff_sel.pol_arst = నిజం;
		ff_దత్తాంశం.has_arst = నిజం;
		ff_దత్తాంశం.val_arst = పోర్టు.arst_విలువ;
		ff_దత్తాంశం.sig_arst = పోర్టు.arst;
		ff_దత్తాంశం.pol_arst = నిజం;
		పోర్టు.arst = State::S0;
	}
	ఒకవేళ (పోర్టు.srst != State::S0) {
		log_assert(!పోర్టు.ce_over_srst);
		ff_sel.has_srst = నిజం;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = పోర్టు.srst;
		ff_sel.pol_srst = నిజం;
		ff_sel.ce_over_srst = అబద్ధం;
		ff_దత్తాంశం.has_srst = నిజం;
		ff_దత్తాంశం.val_srst = పోర్టు.srst_విలువ;
		ff_దత్తాంశం.sig_srst = పోర్టు.srst;
		ff_దత్తాంశం.pol_srst = నిజం;
		ff_దత్తాంశం.ce_over_srst = అబద్ధం;
		పోర్టు.srst = State::S0;
	}
	ff_sel.emit();
	ff_దత్తాంశం.emit();
	module->కూడు_Mux(NEW_ID, prev_దత్తాంశం, new_దత్తాంశం, sel, పోర్టు.దత్తాంశం);
	పోర్టు.దత్తాంశం = new_దత్తాంశం;
	పోర్టు.en = State::S1;
}

శూన్య Mem::emulate_reset(పూర్ణాంకం idx, తార్కిక emu_init, తార్కిక emu_arst, తార్కిక emu_srst, FfInitVals *initvals) {
	స్వయంచాలక &పోర్టు = rd_ports[idx];
	ఒకవేళ (emu_init && !పోర్టు.init_విలువ.is_fully_undef()) {
		Wire *sel = module->కూడు_Wire(NEW_ID);
		Ff_దత్తాంశం ff_sel(module, initvals, NEW_ID);
		Wire *new_దత్తాంశం = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.దత్తాంశం));
		ff_sel.వెడల్పు = 1;
		ff_sel.has_clk = నిజం;
		ff_sel.sig_clk = పోర్టు.clk;
		ff_sel.pol_clk = పోర్టు.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		ఒకవేళ (పోర్టు.en != State::S1) {
			ff_sel.has_ce = నిజం;
			ff_sel.sig_ce = పోర్టు.en;
			ff_sel.pol_ce = నిజం;
			ff_sel.ce_over_srst = పోర్టు.ce_over_srst;
		}
		ఒకవేళ (పోర్టు.arst != State::S0) {
			ff_sel.has_arst = నిజం;
			ff_sel.sig_arst = పోర్టు.arst;
			ff_sel.pol_arst = నిజం;
			ఒకవేళ (emu_arst && పోర్టు.arst_విలువ == పోర్టు.init_విలువ) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				పోర్టు.arst = State::S0;
			} లేకపోతే {
				ff_sel.val_arst = State::S1;
			}
		}
		ఒకవేళ (పోర్టు.srst != State::S0) {
			ff_sel.has_srst = నిజం;
			ff_sel.sig_srst = పోర్టు.srst;
			ff_sel.pol_srst = నిజం;
			ఒకవేళ (emu_srst && పోర్టు.srst_విలువ == పోర్టు.init_విలువ) {
				ff_sel.val_srst = State::S0;
				పోర్టు.srst = State::S0;
			} లేకపోతే {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->కూడు_Mux(NEW_ID, పోర్టు.init_విలువ, new_దత్తాంశం, sel, పోర్టు.దత్తాంశం);
		పోర్టు.దత్తాంశం = new_దత్తాంశం;
		పోర్టు.init_విలువ = Const(State::Sx, Get_కొలత(పోర్టు.దత్తాంశం));
	}
	ఒకవేళ (emu_arst && పోర్టు.arst != State::S0) {
		Wire *sel = module->కూడు_Wire(NEW_ID);
		Ff_దత్తాంశం ff_sel(module, initvals, NEW_ID);
		Wire *new_దత్తాంశం = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.దత్తాంశం));
		ff_sel.వెడల్పు = 1;
		ff_sel.has_clk = నిజం;
		ff_sel.sig_clk = పోర్టు.clk;
		ff_sel.pol_clk = పోర్టు.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ఒకవేళ (పోర్టు.init_విలువ.is_fully_undef())
			ff_sel.val_init = State::Sx;
		లేకపోతే
			ff_sel.val_init = State::S1;
		ఒకవేళ (పోర్టు.en != State::S1) {
			ff_sel.has_ce = నిజం;
			ff_sel.sig_ce = పోర్టు.en;
			ff_sel.pol_ce = నిజం;
			ff_sel.ce_over_srst = పోర్టు.ce_over_srst;
		}
		ff_sel.has_arst = నిజం;
		ff_sel.sig_arst = పోర్టు.arst;
		ff_sel.pol_arst = నిజం;
		ff_sel.val_arst = State::S0;
		ఒకవేళ (పోర్టు.srst != State::S0) {
			ff_sel.has_srst = నిజం;
			ff_sel.sig_srst = పోర్టు.srst;
			ff_sel.pol_srst = నిజం;
			ఒకవేళ (emu_srst && పోర్టు.srst_విలువ == పోర్టు.arst_విలువ) {
				ff_sel.val_srst = State::S0;
				పోర్టు.srst = State::S0;
			} లేకపోతే {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->కూడు_Mux(NEW_ID, పోర్టు.arst_విలువ, new_దత్తాంశం, sel, పోర్టు.దత్తాంశం);
		పోర్టు.దత్తాంశం = new_దత్తాంశం;
		పోర్టు.arst = State::S0;
	}
	ఒకవేళ (emu_srst && పోర్టు.srst != State::S0) {
		Wire *sel = module->కూడు_Wire(NEW_ID);
		Ff_దత్తాంశం ff_sel(module, initvals, NEW_ID);
		Wire *new_దత్తాంశం = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.దత్తాంశం));
		ff_sel.వెడల్పు = 1;
		ff_sel.has_clk = నిజం;
		ff_sel.sig_clk = పోర్టు.clk;
		ff_sel.pol_clk = పోర్టు.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ఒకవేళ (పోర్టు.init_విలువ.is_fully_undef())
			ff_sel.val_init = State::Sx;
		లేకపోతే
			ff_sel.val_init = State::S1;
		ఒకవేళ (పోర్టు.en != State::S1) {
			ff_sel.has_ce = నిజం;
			ff_sel.sig_ce = పోర్టు.en;
			ff_sel.pol_ce = నిజం;
			ff_sel.ce_over_srst = పోర్టు.ce_over_srst;
		}
		ff_sel.has_srst = నిజం;
		ff_sel.sig_srst = పోర్టు.srst;
		ff_sel.pol_srst = నిజం;
		ff_sel.val_srst = State::S0;
		ఒకవేళ (పోర్టు.arst != State::S0) {
			ff_sel.has_arst = నిజం;
			ff_sel.sig_arst = పోర్టు.arst;
			ff_sel.pol_arst = నిజం;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->కూడు_Mux(NEW_ID, పోర్టు.srst_విలువ, new_దత్తాంశం, sel, పోర్టు.దత్తాంశం);
		పోర్టు.దత్తాంశం = new_దత్తాంశం;
		పోర్టు.srst = State::S0;
	}
}

శూన్య Mem::emulate_rd_ce_over_srst(పూర్ణాంకం idx) {
	స్వయంచాలక &పోర్టు = rd_ports[idx];
	log_assert(పోర్టు.clk_enable);
	ఒకవేళ (పోర్టు.en == State::S1 || పోర్టు.srst == State::S0 || !పోర్టు.ce_over_srst) {
		పోర్టు.ce_over_srst = అబద్ధం;
		తిరిగి;
	}
	పోర్టు.ce_over_srst = అబద్ధం;
	పోర్టు.srst = module->And(NEW_ID, పోర్టు.en, పోర్టు.srst);
}

శూన్య Mem::emulate_rd_srst_over_ce(పూర్ణాంకం idx) {
	స్వయంచాలక &పోర్టు = rd_ports[idx];
	log_assert(పోర్టు.clk_enable);
	ఒకవేళ (పోర్టు.en == State::S1 || పోర్టు.srst == State::S0 || పోర్టు.ce_over_srst) {
		పోర్టు.ce_over_srst = నిజం;
		తిరిగి;
	}
	పోర్టు.ce_over_srst = నిజం;
	పోర్టు.en = module->Or(NEW_ID, పోర్టు.en, పోర్టు.srst);
}

తార్కిక Mem::emulate_read_మొదటి_ok() {
	ఒకవేళ (wr_ports.ఖాళీలు())
		తిరిగి అబద్ధం;
	SigSpec clk = wr_ports[0].clk;
	తార్కిక clk_polarity = wr_ports[0].clk_polarity;
	కొరకు (స్వయంచాలక &పోర్టు: wr_ports) {
		ఒకవేళ (!పోర్టు.clk_enable)
			తిరిగి అబద్ధం;
		ఒకవేళ (పోర్టు.clk != clk)
			తిరిగి అబద్ధం;
		ఒకవేళ (పోర్టు.clk_polarity != clk_polarity)
			తిరిగి అబద్ధం;
	}
	తార్కిక దొరికింది_read_మొదటి = అబద్ధం;
	కొరకు (స్వయంచాలక &పోర్టు: rd_ports) {
		ఒకవేళ (!పోర్టు.clk_enable)
			తిరిగి అబద్ధం;
		ఒకవేళ (పోర్టు.clk != clk)
			తిరిగి అబద్ధం;
		ఒకవేళ (పోర్టు.clk_polarity != clk_polarity)
			తిరిగి అబద్ధం;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(wr_ports); j++)
			ఒకవేళ (!పోర్టు.transparency_ముసుగు[j] && !పోర్టు.collision_x_ముసుగు[j])
				దొరికింది_read_మొదటి = నిజం;
	}
	తిరిగి దొరికింది_read_మొదటి;
}

శూన్య Mem::emulate_read_మొదటి(FfInitVals *initvals) {
	log_assert(emulate_read_మొదటి_ok());
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ports); i++)
		కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(wr_ports); j++)
			ఒకవేళ (rd_ports[i].transparency_ముసుగు[j])
				emulate_transparency(j, i, initvals);
	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(rd_ports); i++)
		కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_ముసుగు[j]);
			rd_ports[i].collision_x_ముసుగు[j] = అబద్ధం;
			rd_ports[i].transparency_ముసుగు[j] = నిజం;
		}
	కొరకు (స్వయంచాలక &పోర్టు: wr_ports) {
		Wire *new_దత్తాంశం = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.దత్తాంశం));
		Wire *new_addr = module->కూడు_Wire(NEW_ID, Get_కొలత(పోర్టు.addr));
		స్వయంచాలక compressed = పోర్టు.compress_en();
		Wire *new_en = module->కూడు_Wire(NEW_ID, Get_కొలత(compressed.మొదటి));
		Ff_దత్తాంశం ff_దత్తాంశం(module, initvals, NEW_ID);
		Ff_దత్తాంశం ff_addr(module, initvals, NEW_ID);
		Ff_దత్తాంశం ff_en(module, initvals, NEW_ID);
		ff_దత్తాంశం.వెడల్పు = Get_కొలత(పోర్టు.దత్తాంశం);
		ff_దత్తాంశం.has_clk = నిజం;
		ff_దత్తాంశం.sig_clk = పోర్టు.clk;
		ff_దత్తాంశం.pol_clk = పోర్టు.clk_polarity;
		ff_దత్తాంశం.sig_d = పోర్టు.దత్తాంశం;
		ff_దత్తాంశం.sig_q = new_దత్తాంశం;;
		ff_దత్తాంశం.val_init = Const(State::Sx, ff_దత్తాంశం.వెడల్పు);
		ff_దత్తాంశం.emit();
		ff_addr.వెడల్పు = Get_కొలత(పోర్టు.addr);
		ff_addr.has_clk = నిజం;
		ff_addr.sig_clk = పోర్టు.clk;
		ff_addr.pol_clk = పోర్టు.clk_polarity;
		ff_addr.sig_d = పోర్టు.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.వెడల్పు);
		ff_addr.emit();
		ff_en.వెడల్పు = Get_కొలత(compressed.మొదటి);
		ff_en.has_clk = నిజం;
		ff_en.sig_clk = పోర్టు.clk;
		ff_en.pol_clk = పోర్టు.clk_polarity;
		ff_en.sig_d = compressed.మొదటి;
		ff_en.sig_q = new_en;;
		ఒకవేళ (inits.ఖాళీలు())
			ff_en.val_init = Const(State::Sx, ff_en.వెడల్పు);
		లేకపోతే
			ff_en.val_init = Const(State::S0, ff_en.వెడల్పు);
		ff_en.emit();
		పోర్టు.దత్తాంశం = new_దత్తాంశం;
		పోర్టు.addr = new_addr;
		పోర్టు.en = పోర్టు.decompress_en(compressed.రెండవ, new_en);
	}
}

std::pair<SigSpec, std::సదిశ<పూర్ణాంకం>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::సదిశ<పూర్ణాంకం> swizzle;
	Sig_బిట్టు prev_బిట్టు = en[0];
	పూర్ణాంకం idx = 0;
	కొరకు (స్వయంచాలక &బిట్టు: en) {
		ఒకవేళ (బిట్టు != prev_బిట్టు) {
			sig.append(బిట్టు);
			prev_బిట్టు = బిట్టు;
			idx++;
		}
		swizzle.నెట్టు_వెనుక(idx);
	}
	log_assert(idx + 1 == Get_కొలత(sig));
	తిరిగి {sig, swizzle};
}

SigSpec MemWr::decompress_en(స్థిరరాశి std::సదిశ<పూర్ణాంకం> &swizzle, SigSpec sig) {
	SigSpec res;
	కొరకు (పూర్ణాంకం i: swizzle)
		res.append(sig[i]);
	తిరిగి res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->కొలత), mem->వెడల్పు)
{
	కొరకు(స్థిరరాశి స్వయంచాలక &init : mem->inits) {
		ఒకవేళ(init.en.is_fully_zero()) కొనసాగు;
		log_assert(init.en.కొలత() == దత్తాంశం_వెడల్పు_2);
		ఒకవేళ(init.en.is_fully_ones())
			చొప్పించు_concatenated(init.addr.as_int(), init.దత్తాంశం);
		లేకపోతే {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t పదాలు_2 = init.దత్తాంశం.కొలత() / దత్తాంశం_వెడల్పు_2;
			RTLIL::Const దత్తాంశం = init.దత్తాంశం;
			log_assert(దత్తాంశం.కొలత() % దత్తాంశం_వెడల్పు_2 == 0);
			కొరకు(addr_t i = 0; i < పదాలు_2; i++) {
				RTLIL::Const మునుపటి = (*ఇది)[addr + i];
				కొరకు(పూర్ణాంకం j = 0; j < దత్తాంశం_వెడల్పు_2; j++)
					ఒకవేళ(init.en[j] != State::S1)
						దత్తాంశం.సమితి(దత్తాంశం_వెడల్పు_2 * i + j, మునుపటి[j]);
			}
			చొప్పించు_concatenated(init.addr.as_int(), దత్తాంశం);
		}
	}
}

MemContents::iterator & MemContents::iterator::క్రియాకారకం++() {
	స్వయంచాలక it = జ్ఞాపకం_2->విలువలు_2.పై_bound(_addr);
	ఒకవేళ(it == జ్ఞాపకం_2->విలువలు_2.చివర()) {
		జ్ఞాపకం_2 = శూన్యసూచిక;
		_addr = ~(addr_t) 0;
	} లేకపోతే
		_addr = it->మొదటి;
	తిరిగి *ఇది;
}

శూన్య MemContents::check() {
	log_assert(addr_వెడల్పు_2 > 0 && addr_వెడల్పు_2 < (పూర్ణాంకం)పరిమాణం(addr_t) * 8);
	log_assert(దత్తాంశం_వెడల్పు_2 > 0);
	log_assert(default_విలువ.కొలత() == దత్తాంశం_వెడల్పు_2);
	ఒకవేళ(విలువలు_2.ఖాళీలు()) తిరిగి;
	స్వయంచాలక it = విలువలు_2.begin();
	కొరకు(;;) {
		log_assert(!it->రెండవ.ఖాళీలు());
		log_assert(it->రెండవ.కొలత() % దత్తాంశం_వెడల్పు_2 == 0);
		స్వయంచాలక end1 = range_చివర(it);
		log_assert(_range_begin(it) < (addr_t)(1<<addr_వెడల్పు_2));
		log_assert(end1 <= (addr_t)(1<<addr_వెడల్పు_2));
		ఒకవేళ(++it == విలువలు_2.చివర())
			ఆపు;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

తార్కిక MemContents::range_కలిగియుంది(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) స్థిరరాశి {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	తిరిగి it != విలువలు_2.చివర() && addr - _range_begin(it) < range_కొలత(it);
}


తార్కిక MemContents::range_కలిగియుంది(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t చివర_addr) స్థిరరాశి {
	// note that we assume begin_addr <= end_addr
	తిరిగి it != విలువలు_2.చివర() && _range_begin(it) <= begin_addr && చివర_addr - _range_begin(it) <= range_కొలత(it);
}

తార్కిక MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t చివర_addr) స్థిరరాశి {
	ఒకవేళ(it == విలువలు_2.చివర() || begin_addr >= చివర_addr)
		తిరిగి అబద్ధం;
	స్వయంచాలక top1 = range_చివర(it) - 1;
	స్వయంచాలక top2 = చివర_addr - 1;
	తిరిగి !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) స్థిరరాశి {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<addr_వెడల్పు_2));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	స్వయంచాలక it = const_cast<std::map<addr_t, RTLIL::Const> &>(విలువలు_2).పై_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	ఒకవేళ(it == విలువలు_2.begin())
		తిరిగి it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	స్వయంచాలక it_prev = std::తదుపరి(it, -1);
	ఒకవేళ(range_కలిగియుంది(it_prev, addr))
		తిరిగి it_prev;
	లేకపోతే
		తిరిగి it;
}

RTLIL::Const MemContents::క్రియాకారకం[](addr_t addr) స్థిరరాశి {
	స్వయంచాలక it = _range_at(addr);
	ఒకవేళ(range_కలిగియుంది(it, addr))
		తిరిగి it->రెండవ.వెలికితీయి(range_స్థానభ్రంశం(it, addr), దత్తాంశం_వెడల్పు_2);
	లేకపోతే
		తిరిగి default_విలువ;
}

MemContents::addr_t MemContents::లెక్క_range(addr_t begin_addr, addr_t చివర_addr) స్థిరరాశి {
	addr_t లెక్క = 0;
	కొరకు(స్వయంచాలక it = _range_at(begin_addr); _range_overlaps(it, begin_addr, చివర_addr); it++) {
		స్వయంచాలక మొదటి = std::max(_range_begin(it), begin_addr);
		స్వయంచాలక చివరి = std::min(range_చివర(it), చివర_addr);
		లెక్క += చివరి - మొదటి;
	}
	తిరిగి లెక్క;
}

శూన్య MemContents::clear_range(addr_t begin_addr, addr_t చివర_addr) {
	ఒకవేళ(begin_addr >= చివర_addr) తిరిగి;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	స్వయంచాలక begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	స్వయంచాలక చివర_it = విలువలు_2.పై_bound(చివర_addr - 1);
	ఒకవేళ(begin_it == చివర_it)
		తిరిగి; // nothing to do
	// the last iterator affected is one before the first one not affected
	స్వయంచాలక చివరి_it = std::తదుపరి(చివర_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	ఒకవేళ(range_కలిగియుంది(చివరి_it, చివర_addr - 1)) {
		స్వయంచాలక new_begin = చివర_addr;
		స్వయంచాలక చివర = range_చివర(చివరి_it);
		// if there is data past the end address, preserve it by creating a new range
		ఒకవేళ(new_begin != చివర)
			చివర_it = విలువలు_2.emplace_hint(చివరి_it, new_begin, చివరి_it->రెండవ.వెలికితీయి(range_స్థానభ్రంశం(చివరి_it, new_begin), (range_చివర(చివరి_it) - new_begin) * దత్తాంశం_వెడల్పు_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	ఒకవేళ(range_కలిగియుంది(begin_it, begin_addr)) {
		స్వయంచాలక new_చివర = begin_addr;
		// if there is data before the start address, truncate but don't delete
		ఒకవేళ(new_చివర != begin_it->మొదటి) {
			begin_it->రెండవ.extu(range_స్థానభ్రంశం(begin_it, new_చివర));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	విలువలు_2.erase(begin_it, చివర_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t చివర_addr) {
	ఒకవేళ(begin_addr >= చివర_addr)
		తిరిగి విలువలు_2.చివర(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	స్వయంచాలక క్రింది_it = begin_addr == 0 ? విలువలు_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	ఒకవేళ (range_కలిగియుంది(క్రింది_it, begin_addr, చివర_addr))
		తిరిగి క్రింది_it;
	// find the first range containing any addr >= end_addr
	స్వయంచాలక పై_it = _range_at(చివర_addr);
	// check if either of the two ranges we just found touch our range
	తార్కిక క్రింది_touch = begin_addr > 0 && range_కలిగియుంది(క్రింది_it, begin_addr - 1);
	తార్కిక పై_touch = range_కలిగియుంది(పై_it, చివర_addr);
	ఒకవేళ (క్రింది_touch && పై_touch) {
		log_assert (క్రింది_it != పై_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		స్వయంచాలక పై_చివర = range_చివర(పై_it);
		// make range bigger (maybe reserve here instead of resize?)
		క్రింది_it->రెండవ.resize(range_స్థానభ్రంశం(క్రింది_it, పై_చివర), State::Sx);
		// copy only the data beyond our range
		std::copy(range_దత్తాంశం(పై_it, చివర_addr), range_దత్తాంశం(పై_it, పై_చివర), range_దత్తాంశం(క్రింది_it, చివర_addr));
		// keep lower_it, but delete upper_it
		విలువలు_2.erase(std::తదుపరి(క్రింది_it), std::తదుపరి(పై_it));
		తిరిగి క్రింది_it;
	} లేకపోతే ఒకవేళ (క్రింది_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		క్రింది_it->రెండవ.resize(range_స్థానభ్రంశం(క్రింది_it, చివర_addr), State::Sx);
		// keep lower_it and upper_it
		విలువలు_2.erase(std::తదుపరి(క్రింది_it), పై_it);
		తిరిగి క్రింది_it;
	} లేకపోతే ఒకవేళ (పై_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const దత్తాంశం(State::Sx, (_range_begin(పై_it) - begin_addr) * దత్తాంశం_వెడల్పు_2);
		దత్తాంశం.append(std::move(పై_it->రెండవ));
		// delete lower_it and upper_it, then reinsert
		విలువలు_2.erase(క్రింది_it, std::తదుపరి(పై_it));
		తిరిగి విలువలు_2.emplace(begin_addr, std::move(దత్తాంశం)).మొదటి;
	} లేకపోతే {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		విలువలు_2.erase(క్రింది_it, పై_it);
		తిరిగి విలువలు_2.emplace(begin_addr, RTLIL::Const(State::Sx, (చివర_addr - begin_addr) * దత్తాంశం_వెడల్పు_2)).మొదటి;
	}
}

శూన్య MemContents::చొప్పించు_concatenated(addr_t addr, RTLIL::Const స్థిరరాశి &విలువలు) {
	addr_t పదాలు_2 = (విలువలు.కొలత() + దత్తాంశం_వెడల్పు_2 - 1) / దత్తాంశం_వెడల్పు_2;
	log_assert(addr < (addr_t)(1<<addr_వెడల్పు_2));
	log_assert(పదాలు_2 <= (addr_t)(1<<addr_వెడల్పు_2) - addr);
	స్వయంచాలక it = _reserve_range(addr, addr + పదాలు_2);
	స్వయంచాలక గమ్యం_begin = range_దత్తాంశం(it, addr);
	std::copy(విలువలు.begin(), విలువలు.చివర(), గమ్యం_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(గమ్యం_begin + విలువలు.కొలత(), గమ్యం_begin + పదాలు_2 * దత్తాంశం_వెడల్పు_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const స్థిరరాశి &మాట) {
	స్వయంచాలక from_చివర = మాట.కొలత() <= దత్తాంశం_వెడల్పు_2 ? మాట.చివర() : మాట.begin() + దత్తాంశం_వెడల్పు_2;
	స్వయంచాలక గమ్యం_చివర = std::copy(మాట.begin(), from_చివర, it);
	స్వయంచాలక it_తదుపరి = std::తదుపరి(it, దత్తాంశం_వెడల్పు_2);
	std::fill(గమ్యం_చివర, it_తదుపరి, State::S0);
	తిరిగి it_తదుపరి;
}
