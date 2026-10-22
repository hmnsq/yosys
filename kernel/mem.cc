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

#समावेश "kernel/mem.h"
#समावेश "kernel/ff.h"

USING_YOSYS_NAMESPACE

व्योम Mem::हटाओ() {
	अगर (cell) {
		module->हटाओ(cell);
		cell = nullptr;
	}
	अगर (mem) {
		module->memories.erase(mem->name);
		मिटाओ mem;
		mem = nullptr;
	}
	क्रम (स्वतः &port : rd_ports) {
		अगर (port.cell) {
			module->हटाओ(port.cell);
			port.cell = nullptr;
		}
	}
	क्रम (स्वतः &port : wr_ports) {
		अगर (port.cell) {
			module->हटाओ(port.cell);
			port.cell = nullptr;
		}
	}
	क्रम (स्वतः &init : inits) {
		अगर (init.cell) {
			module->हटाओ(init.cell);
			init.cell = nullptr;
		}
	}
}

व्योम Mem::emit() {
	check();
	std::vector<पूर्णांक> rd_बायाँ;
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_ports); i++) {
		स्वतः &port = rd_ports[i];
		अगर (port.removed) {
			अगर (port.cell) {
				module->हटाओ(port.cell);
			}
		} अन्यथा {
			rd_बायाँ.धकेलो_back(i);
		}
	}
	std::vector<पूर्णांक> wr_बायाँ;
	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++) {
		स्वतः &port = wr_ports[i];
		अगर (port.removed) {
			अगर (port.cell) {
				module->हटाओ(port.cell);
			}
		} अन्यथा {
			wr_बायाँ.धकेलो_back(i);
		}
	}
	std::vector<पूर्णांक> init_बायाँ;
	क्रम (पूर्णांक i = 0; i < Get_आकार(inits); i++) {
		स्वतः &init = inits[i];
		अगर (init.removed) {
			अगर (init.cell) {
				module->हटाओ(init.cell);
			}
		} अन्यथा {
			init_बायाँ.धकेलो_back(i);
		}
	}
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_बायाँ); i++)
		अगर (i != rd_बायाँ[i])
			std::swap(rd_ports[i], rd_ports[rd_बायाँ[i]]);
	rd_ports.resize(Get_आकार(rd_बायाँ));
	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_बायाँ); i++)
		अगर (i != wr_बायाँ[i])
			std::swap(wr_ports[i], wr_ports[wr_बायाँ[i]]);
	wr_ports.resize(Get_आकार(wr_बायाँ));
	क्रम (पूर्णांक i = 0; i < Get_आकार(init_बायाँ); i++)
		अगर (i != init_बायाँ[i])
			std::swap(inits[i], inits[init_बायाँ[i]]);
	inits.resize(Get_आकार(init_बायाँ));

	क्रम (स्वतः &port : rd_ports) {
		क्रम (पूर्णांक i = 0; i < Get_आकार(wr_बायाँ); i++) {
			port.transparency_mask[i] = port.transparency_mask[wr_बायाँ[i]];
			port.collision_x_mask[i] = port.collision_x_mask[wr_बायाँ[i]];
		}
		port.transparency_mask.resize(Get_आकार(wr_बायाँ));
		port.collision_x_mask.resize(Get_आकार(wr_बायाँ));
	}
	क्रम (स्वतः &port : wr_ports) {
		क्रम (पूर्णांक i = 0; i < Get_आकार(wr_बायाँ); i++)
			port.priority_mask[i] = port.priority_mask[wr_बायाँ[i]];
		port.priority_mask.resize(Get_आकार(wr_बायाँ));
	}

	अगर (packed) {
		अगर (mem) {
			module->memories.erase(mem->name);
			मिटाओ mem;
			mem = nullptr;
		}
		अगर (!cell) {
			अगर (memid.empty())
				memid = PooledName(module->design, module->twines().add(NEW_ID));
			cell = module->addCell(memid, ID($mem_v2));
		}
		cell->type = ID($mem_v2);
		cell->attributes = attributes;
		cell->parameters[ID::MEMID] = Const(memid.str());
		cell->parameters[ID::WIDTH] = Const(width);
		cell->parameters[ID::OFFSET] = Const(start_offset);
		cell->parameters[ID::आकार_2] = Const(आकार);
		SigSpec rd_clk, rd_en, rd_addr, rd_तथ्य;
		SigSpec wr_clk, wr_en, wr_addr, wr_तथ्य;
		SigSpec rd_arst, rd_srst;
		पूर्णांक abits = 0;
		क्रम (स्वतः &port : rd_ports)
			abits = std::max(abits, Get_आकार(port.addr));
		क्रम (स्वतः &port : wr_ports)
			abits = std::max(abits, Get_आकार(port.addr));
		cell->parameters[ID::ABITS] = Const(abits);
		std::vector<पूर्णांक> wr_port_xlat;
		क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++)
			क्रम (पूर्णांक j = 0; j < (1 << wr_ports[i].wide_log2); j++)
				wr_port_xlat.धकेलो_back(i);
		Const::Builder rd_wide_continuation_builder;
		Const::Builder rd_clk_enable_builder;
		Const::Builder rd_clk_polarity_builder;
		Const::Builder rd_transparency_mask_builder;
		Const::Builder rd_collision_x_mask_builder;
		Const::Builder rd_ce_over_srst_builder;
		Const::Builder rd_arst_मूल्य_builder;
		Const::Builder rd_srst_मूल्य_builder;
		Const::Builder rd_init_मूल्य_builder;
		क्रम (स्वतः &port : rd_ports) {
			क्रम (स्वतः attr: port.attributes)
				अगर (!cell->has_attribute(attr.first))
					cell->attributes.जोड़ो(attr);
			अगर (port.cell) {
				module->हटाओ(port.cell);
				port.cell = nullptr;
			}
			क्रम (पूर्णांक sub = 0; sub < (1 << port.wide_log2); sub++)
			{
				rd_wide_continuation_builder.धकेलो_back(State(sub != 0));
				rd_clk_enable_builder.धकेलो_back(State(port.clk_enable));
				rd_clk_polarity_builder.धकेलो_back(State(port.clk_polarity));
				rd_ce_over_srst_builder.धकेलो_back(State(port.ce_over_srst));
				rd_clk.append(port.clk);
				rd_arst.append(port.arst);
				rd_srst.append(port.srst);
				rd_en.append(port.en);
				SigSpec addr = port.sub_addr(sub);
				addr.extend_u0(abits, false);
				rd_addr.append(addr);
				log_assert(Get_आकार(addr) == abits);
				क्रम (स्वतः idx : wr_port_xlat) {
					rd_transparency_mask_builder.धकेलो_back(State(bool(port.transparency_mask[idx])));
					rd_collision_x_mask_builder.धकेलो_back(State(bool(port.collision_x_mask[idx])));
				}
			}
			rd_तथ्य.append(port.तथ्य);
			क्रम (स्वतः bit : port.arst_मूल्य)
				rd_arst_मूल्य_builder.धकेलो_back(bit);
			क्रम (स्वतः bit : port.srst_मूल्य)
				rd_srst_मूल्य_builder.धकेलो_back(bit);
			क्रम (स्वतः bit : port.init_मूल्य)
				rd_init_मूल्य_builder.धकेलो_back(bit);
		}
		Const rd_wide_continuation = rd_wide_continuation_builder.build();
		Const rd_clk_enable = rd_clk_enable_builder.build();
		Const rd_clk_polarity = rd_clk_polarity_builder.build();
		Const rd_transparency_mask = rd_transparency_mask_builder.build();
		Const rd_collision_x_mask = rd_collision_x_mask_builder.build();
		Const rd_ce_over_srst = rd_ce_over_srst_builder.build();
		Const rd_arst_मूल्य = rd_arst_मूल्य_builder.build();
		Const rd_srst_मूल्य = rd_srst_मूल्य_builder.build();
		Const rd_init_मूल्य = rd_init_मूल्य_builder.build();
		अगर (rd_ports.empty()) {
			rd_wide_continuation = State::S0;
			rd_clk_enable = State::S0;
			rd_clk_polarity = State::S0;
			rd_ce_over_srst = State::S0;
			rd_arst_मूल्य = State::S0;
			rd_srst_मूल्य = State::S0;
			rd_init_मूल्य = State::S0;
		}
		अगर (rd_ports.empty() || wr_ports.empty()) {
			rd_transparency_mask = State::S0;
			rd_collision_x_mask = State::S0;
		}
		cell->parameters[ID::RD_PORTS] = Const(Get_आकार(rd_clk));
		cell->parameters[ID::RD_CLK_ENABLE] = rd_clk_enable;
		cell->parameters[ID::RD_CLK_POLARITY] = rd_clk_polarity;
		cell->parameters[ID::RD_TRANSPARENCY_MASK] = rd_transparency_mask;
		cell->parameters[ID::RD_COLLISION_X_MASK] = rd_collision_x_mask;
		cell->parameters[ID::RD_WIDE_CONTINUATION] = rd_wide_continuation;
		cell->parameters[ID::RD_CE_OVER_SRST] = rd_ce_over_srst;
		cell->parameters[ID::RD_ARST_मूल्य] = rd_arst_मूल्य;
		cell->parameters[ID::RD_SRST_मूल्य] = rd_srst_मूल्य;
		cell->parameters[ID::RD_INIT_मूल्य] = rd_init_मूल्य;
		cell->parameters.erase(ID::RD_TRANSPARENT);
		cell->setPort(ID::RD_CLK, rd_clk);
		cell->setPort(ID::RD_EN, rd_en);
		cell->setPort(ID::RD_ARST, rd_arst);
		cell->setPort(ID::RD_SRST, rd_srst);
		cell->setPort(ID::RD_ADDR, rd_addr);
		cell->setPort(ID::RD_तथ्य, rd_तथ्य);
		Const::Builder wr_wide_continuation_builder;
		Const::Builder wr_clk_enable_builder;
		Const::Builder wr_clk_polarity_builder;
		Const::Builder wr_priority_mask_builder;
		क्रम (स्वतः &port : wr_ports) {
			क्रम (स्वतः attr: port.attributes)
				अगर (!cell->has_attribute(attr.first))
					cell->attributes.जोड़ो(attr);
			अगर (port.cell) {
				module->हटाओ(port.cell);
				port.cell = nullptr;
			}
			क्रम (पूर्णांक sub = 0; sub < (1 << port.wide_log2); sub++)
			{
				wr_wide_continuation_builder.धकेलो_back(State(sub != 0));
				wr_clk_enable_builder.धकेलो_back(State(port.clk_enable));
				wr_clk_polarity_builder.धकेलो_back(State(port.clk_polarity));
				wr_clk.append(port.clk);
				क्रम (स्वतः idx : wr_port_xlat)
					wr_priority_mask_builder.धकेलो_back(State(bool(port.priority_mask[idx])));
				SigSpec addr = port.sub_addr(sub);
				addr.extend_u0(abits, false);
				wr_addr.append(addr);
				log_assert(Get_आकार(addr) == abits);
			}
			wr_en.append(port.en);
			wr_तथ्य.append(port.तथ्य);
		}
		Const wr_wide_continuation = wr_wide_continuation_builder.build();
		Const wr_clk_enable = wr_clk_enable_builder.build();
		Const wr_clk_polarity = wr_clk_polarity_builder.build();
		Const wr_priority_mask = wr_priority_mask_builder.build();
		अगर (wr_ports.empty()) {
			wr_wide_continuation = State::S0;
			wr_clk_enable = State::S0;
			wr_clk_polarity = State::S0;
			wr_priority_mask = State::S0;
		}
		cell->parameters[ID::WR_PORTS] = Const(Get_आकार(wr_clk));
		cell->parameters[ID::WR_CLK_ENABLE] = wr_clk_enable;
		cell->parameters[ID::WR_CLK_POLARITY] = wr_clk_polarity;
		cell->parameters[ID::WR_PRIORITY_MASK] = wr_priority_mask;
		cell->parameters[ID::WR_WIDE_CONTINUATION] = wr_wide_continuation;
		cell->setPort(ID::WR_CLK, wr_clk);
		cell->setPort(ID::WR_EN, wr_en);
		cell->setPort(ID::WR_ADDR, wr_addr);
		cell->setPort(ID::WR_तथ्य, wr_तथ्य);
		क्रम (स्वतः &init : inits) {
			क्रम (स्वतः attr: init.attributes)
				अगर (!cell->has_attribute(attr.first))
					cell->attributes.जोड़ो(attr);
			अगर (init.cell) {
				module->हटाओ(init.cell);
				init.cell = nullptr;
			}
		}
		cell->parameters[ID::INIT] = get_init_तथ्य();
	} अन्यथा {
		अगर (cell) {
			module->हटाओ(cell);
			cell = nullptr;
		}
		अगर (!mem) {
			अगर (memid.empty())
				memid = PooledName(module->design, module->twines().add(NEW_ID));
			mem = module->addMemory(memid);
		}
		mem->width = width;
		mem->start_offset = start_offset;
		mem->आकार = आकार;
		mem->attributes = attributes;
		क्रम (स्वतः &port : rd_ports) {
			अगर (!port.cell)
				port.cell = module->addCell(NEW_ID, ID($memrd_v2));
			port.cell->type = ID($memrd_v2);
			port.cell->attributes = port.attributes;
			port.cell->parameters[ID::MEMID] = memid.str();
			port.cell->parameters[ID::ABITS] = Get_आकार(port.addr);
			port.cell->parameters[ID::WIDTH] = width << port.wide_log2;
			port.cell->parameters[ID::CLK_ENABLE] = port.clk_enable;
			port.cell->parameters[ID::CLK_POLARITY] = port.clk_polarity;
			port.cell->parameters[ID::CE_OVER_SRST] = port.ce_over_srst;
			port.cell->parameters[ID::ARST_मूल्य] = port.arst_मूल्य;
			port.cell->parameters[ID::SRST_मूल्य] = port.srst_मूल्य;
			port.cell->parameters[ID::INIT_मूल्य] = port.init_मूल्य;
			port.cell->parameters[ID::TRANSPARENCY_MASK] = port.transparency_mask;
			port.cell->parameters[ID::COLLISION_X_MASK] = port.collision_x_mask;
			port.cell->parameters.erase(ID::TRANSPARENT);
			port.cell->setPort(ID::CLK, port.clk);
			port.cell->setPort(ID::EN, port.en);
			port.cell->setPort(ID::ARST, port.arst);
			port.cell->setPort(ID::SRST, port.srst);
			port.cell->setPort(ID::ADDR, port.addr);
			port.cell->setPort(ID::तथ्य_2, port.तथ्य);
		}
		पूर्णांक idx = 0;
		क्रम (स्वतः &port : wr_ports) {
			अगर (!port.cell)
				port.cell = module->addCell(NEW_ID, ID($memwr_v2));
			port.cell->type = ID($memwr_v2);
			port.cell->attributes = port.attributes;
			अगर (port.cell->parameters.गिनती(ID::PRIORITY))
				port.cell->parameters.erase(ID::PRIORITY);
			port.cell->parameters[ID::MEMID] = memid.str();
			port.cell->parameters[ID::ABITS] = Get_आकार(port.addr);
			port.cell->parameters[ID::WIDTH] = width << port.wide_log2;
			port.cell->parameters[ID::CLK_ENABLE] = port.clk_enable;
			port.cell->parameters[ID::CLK_POLARITY] = port.clk_polarity;
			port.cell->parameters[ID::PORTID] = idx++;
			port.cell->parameters[ID::PRIORITY_MASK] = port.priority_mask;
			port.cell->setPort(ID::CLK, port.clk);
			port.cell->setPort(ID::EN, port.en);
			port.cell->setPort(ID::ADDR, port.addr);
			port.cell->setPort(ID::तथ्य_2, port.तथ्य);
		}
		idx = 0;
		क्रम (स्वतः &init : inits) {
			bool v2 = !init.en.is_fully_ones();
			अगर (!init.cell)
				init.cell = module->addCell(NEW_ID, v2 ? ID($meminit_v2) : ID($meminit));
			अन्यथा {
				अगर (!v2)
					init.cell->unsetPort(ID::EN);
				init.cell->type = v2 ? ID($meminit_v2) : ID($meminit);
			}
			init.cell->attributes = init.attributes;
			init.cell->parameters[ID::MEMID] = memid.str();
			init.cell->parameters[ID::ABITS] = Get_आकार(init.addr);
			init.cell->parameters[ID::WIDTH] = width;
			init.cell->parameters[ID::शब्दगण] = Get_आकार(init.तथ्य) / width;
			init.cell->parameters[ID::PRIORITY] = idx++;
			init.cell->setPort(ID::ADDR, init.addr);
			init.cell->setPort(ID::तथ्य_2, init.तथ्य);
			अगर (v2)
				init.cell->setPort(ID::EN, init.en);
		}
	}
}

व्योम Mem::clear_inits() {
	क्रम (स्वतः &init : inits)
		init.removed = true;
}

व्योम Mem::coalesce_inits() {
	// start address -> end address
	std::map<पूर्णांक, पूर्णांक> chunks;
	// Figure out chunk boundaries.
	क्रम (स्वतः &init : inits) {
		अगर (init.removed)
			जारी;
		bool valid = false;
		क्रम (स्वतः bit : init.en)
			अगर (bit == State::S1)
				valid = true;
		अगर (!valid) {
			init.removed = true;
			जारी;
		}
		पूर्णांक addr = init.addr.as_int();
		पूर्णांक addr_e = addr + Get_आकार(init.तथ्य) / width;
		स्वतः it_e = chunks.upper_bound(addr_e);
		स्वतः it = it_e;
		जबतक (it != chunks.begin()) {
			--it;
			अगर (it->second < addr) {
				++it;
				अवरोध;
			}
		}
		अगर (it == it_e) {
			// No overlapping inits — add this one to index.
			chunks[addr] = addr_e;
		} अन्यथा {
			// We have an overlap — all chunks in the [it, it_e)
			// range will be merged with this init.
			अगर (it->first < addr)
				addr = it->first;
			स्वतः it_last = it_e;
			it_last--;
			अगर (it_last->second > addr_e)
				addr_e = it_last->second;
			chunks.erase(it, it_e);
			chunks[addr] = addr_e;
		}
	}
	// Group inits by the chunk they belong to.
	dict<पूर्णांक, std::vector<पूर्णांक>> inits_by_chunk;
	क्रम (पूर्णांक i = 0; i < Get_आकार(inits); i++) {
		स्वतः &init = inits[i];
		अगर (init.removed)
			जारी;
		स्वतः it = chunks.upper_bound(init.addr.as_int());
		--it;
		inits_by_chunk[it->first].धकेलो_back(i);
		पूर्णांक addr = init.addr.as_int();
		पूर्णांक addr_e = addr + Get_आकार(init.तथ्य) / width;
		log_assert(addr >= it->first && addr_e <= it->second);
	}
	// Process each chunk.
	क्रम (स्वतः &it : inits_by_chunk) {
		पूर्णांक caddr = it.first;
		पूर्णांक caddr_e = chunks[caddr];
		स्वतः &chunk_inits = it.second;
		अगर (Get_आकार(chunk_inits) == 1) {
			स्वतः &init = inits[chunk_inits[0]];
			अगर (!init.en.is_fully_ones()) {
				क्रम (पूर्णांक i = 0; i < Get_आकार(init.तथ्य); i++)
					अगर (init.en[i % width] != State::S1)
						init.तथ्य.set(i, State::Sx);
				init.en = Const(State::S1, width);
			}
			जारी;
		}
		Const cdata(State::Sx, (caddr_e - caddr) * width);
		क्रम (पूर्णांक idx : chunk_inits) {
			स्वतः &init = inits[idx];
			पूर्णांक offset = (init.addr.as_int() - caddr) * width;
			log_assert(offset >= 0);
			log_assert(offset + Get_आकार(init.तथ्य) <= Get_आकार(cdata));
			क्रम (पूर्णांक i = 0; i < Get_आकार(init.तथ्य); i++)
				अगर (init.en[i % width] == State::S1)
					cdata.set(i+offset, init.तथ्य[i]);
			init.removed = true;
		}
		MemInit new_init;
		new_init.addr = caddr;
		new_init.तथ्य = cdata;
		new_init.en = Const(State::S1, width);
		inits.धकेलो_back(new_init);
	}
}

Const Mem::get_init_तथ्य() स्थिर {
	Const init_तथ्य(State::Sx, width * आकार);
	क्रम (स्वतः &init : inits) {
		अगर (init.removed)
			जारी;
		पूर्णांक offset = (init.addr.as_int() - start_offset) * width;
		क्रम (पूर्णांक i = 0; i < Get_आकार(init.तथ्य); i++)
			अगर (0 <= i+offset && i+offset < Get_आकार(init_तथ्य) && init.en[i % width] == State::S1)
				init_तथ्य.set(i+offset, init.तथ्य[i]);
	}
	वापस init_तथ्य;
}

व्योम Mem::check() {
	पूर्णांक max_wide_log2 = 0;
	क्रम (स्वतः &port : rd_ports) {
		अगर (port.removed)
			जारी;
		log_assert(Get_आकार(port.clk) == 1);
		log_assert(Get_आकार(port.en) == 1);
		log_assert(Get_आकार(port.arst) == 1);
		log_assert(Get_आकार(port.srst) == 1);
		log_assert(Get_आकार(port.addr) >= port.wide_log2);
		log_assert(Get_आकार(port.तथ्य) == (width << port.wide_log2));
		log_assert(Get_आकार(port.init_मूल्य) == (width << port.wide_log2));
		log_assert(Get_आकार(port.arst_मूल्य) == (width << port.wide_log2));
		log_assert(Get_आकार(port.srst_मूल्य) == (width << port.wide_log2));
		अगर (!port.clk_enable) {
			log_assert(port.en == State::S1);
			log_assert(port.arst == State::S0);
			log_assert(port.srst == State::S0);
		}
		क्रम (पूर्णांक j = 0; j < port.wide_log2; j++) {
			log_assert(port.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, port.wide_log2);
		log_assert(Get_आकार(port.transparency_mask) == Get_आकार(wr_ports));
		log_assert(Get_आकार(port.collision_x_mask) == Get_आकार(wr_ports));
		क्रम (पूर्णांक j = 0; j < Get_आकार(wr_ports); j++) {
			स्वतः &wport = wr_ports[j];
			अगर ((port.transparency_mask[j] || port.collision_x_mask[j]) && !wport.removed) {
				log_assert(port.clk_enable);
				log_assert(wport.clk_enable);
				log_assert(port.clk == wport.clk);
				log_assert(port.clk_polarity == wport.clk_polarity);
			}
			log_assert(!port.transparency_mask[j] || !port.collision_x_mask[j]);
		}
	}
	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++) {
		स्वतः &port = wr_ports[i];
		अगर (port.removed)
			जारी;
		log_assert(Get_आकार(port.clk) == 1);
		log_assert(Get_आकार(port.en) == (width << port.wide_log2));
		log_assert(Get_आकार(port.तथ्य) == (width << port.wide_log2));
		log_assert(Get_आकार(port.addr) >= port.wide_log2);
		क्रम (पूर्णांक j = 0; j < port.wide_log2; j++) {
			log_assert(port.addr[j] == State::S0);
		}
		max_wide_log2 = std::max(max_wide_log2, port.wide_log2);
		log_assert(Get_आकार(port.priority_mask) == Get_आकार(wr_ports));
		क्रम (पूर्णांक j = 0; j < Get_आकार(wr_ports); j++) {
			स्वतः &wport = wr_ports[j];
			अगर (port.priority_mask[j] && !wport.removed) {
				log_assert(j < i);
				log_assert(port.clk_enable == wport.clk_enable);
				अगर (port.clk_enable) {
					log_assert(port.clk == wport.clk);
					log_assert(port.clk_polarity == wport.clk_polarity);
				}
			}
		}
	}
	पूर्णांक mask = (1 << max_wide_log2) - 1;
	log_assert(!(start_offset & mask));
	log_assert(!(आकार & mask));
	log_assert(width != 0);
}

namespace {

	काष्ठा Mem_सूचकांक {
		dict<std::string, pool<Cell *>> rd_ports;
		dict<std::string, pool<Cell *>> wr_ports;
		dict<std::string, pool<Cell *>> inits;
		Mem_सूचकांक (Module *module) {
			क्रम (स्वतः cell: module->cells()) {
				अगर (cell->type.in(ID($memwr), ID($memwr_v2)))
					wr_ports[cell->parameters.at(ID::MEMID).decode_string()].जोड़ो(cell);
				अन्यथा अगर (cell->type.in(ID($memrd), ID($memrd_v2)))
					rd_ports[cell->parameters.at(ID::MEMID).decode_string()].जोड़ो(cell);
				अन्यथा अगर (cell->type.in(ID($meminit), ID($meminit_v2)))
					inits[cell->parameters.at(ID::MEMID).decode_string()].जोड़ो(cell);
			}
		}
	};

	Mem mem_from_memory(Module *module, RTLIL::Memory *mem, स्थिर Mem_सूचकांक &सूचकांक) {
		std::string memid = mem->name.str();
		Mem res(module, mem->name, mem->width, mem->start_offset, mem->आकार);
		res.packed = false;
		res.mem = mem;
		res.attributes = mem->attributes;
		std::vector<bool> rd_transparent;
		std::vector<पूर्णांक> wr_portid;
		अगर (सूचकांक.rd_ports.गिनती(memid)) {
			क्रम (स्वतः cell : सूचकांक.rd_ports.at(memid)) {
				MemRd mrd;
				bool is_compat = cell->type == ID($memrd);
				mrd.cell = cell;
				mrd.attributes = cell->attributes;
				mrd.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mrd.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mrd.clk = cell->getPort(ID::CLK);
				mrd.en = cell->getPort(ID::EN);
				mrd.addr = cell->getPort(ID::ADDR);
				mrd.तथ्य = cell->getPort(ID::तथ्य_2);
				mrd.wide_log2 = ceil_log2(Get_आकार(mrd.तथ्य) / mem->width);
				bool transparent = false;
				अगर (is_compat) {
					transparent = cell->parameters.at(ID::TRANSPARENT).as_bool();
					mrd.ce_over_srst = false;
					mrd.arst_मूल्य = Const(State::Sx, mem->width << mrd.wide_log2);
					mrd.srst_मूल्य = Const(State::Sx, mem->width << mrd.wide_log2);
					mrd.init_मूल्य = Const(State::Sx, mem->width << mrd.wide_log2);
					mrd.srst = State::S0;
					mrd.arst = State::S0;
					अगर (!mrd.clk_enable) {
						// Fix some patterns that we'll allow for backwards compatibility,
						// but don't want to see moving forwards: async transparent
						// ports (inherently meaningless) and async ports without
						// const 1 tied to EN bit (which may mean a latch in the future).
						transparent = false;
						अगर (mrd.en == State::Sx)
							mrd.en = State::S1;
					}
				} अन्यथा {
					mrd.ce_over_srst = cell->parameters.at(ID::CE_OVER_SRST).as_bool();
					mrd.arst_मूल्य = cell->parameters.at(ID::ARST_मूल्य);
					mrd.srst_मूल्य = cell->parameters.at(ID::SRST_मूल्य);
					mrd.init_मूल्य = cell->parameters.at(ID::INIT_मूल्य);
					mrd.arst = cell->getPort(ID::ARST);
					mrd.srst = cell->getPort(ID::SRST);
				}
				res.rd_ports.धकेलो_back(mrd);
				rd_transparent.धकेलो_back(transparent);
			}
		}
		अगर (सूचकांक.wr_ports.गिनती(memid)) {
			std::vector<std::pair<पूर्णांक, MemWr>> ports;
			क्रम (स्वतः cell : सूचकांक.wr_ports.at(memid)) {
				MemWr mwr;
				bool is_compat = cell->type == ID($memwr);
				mwr.cell = cell;
				mwr.attributes = cell->attributes;
				mwr.clk_enable = cell->parameters.at(ID::CLK_ENABLE).as_bool();
				mwr.clk_polarity = cell->parameters.at(ID::CLK_POLARITY).as_bool();
				mwr.clk = cell->getPort(ID::CLK);
				mwr.en = cell->getPort(ID::EN);
				mwr.addr = cell->getPort(ID::ADDR);
				mwr.तथ्य = cell->getPort(ID::तथ्य_2);
				mwr.wide_log2 = ceil_log2(Get_आकार(mwr.तथ्य) / mem->width);
				ports.धकेलो_back(std::make_pair(cell->parameters.at(is_compat ? ID::PRIORITY : ID::PORTID).as_int(), mwr));
			}
			std::sort(ports.begin(), ports.end(), [](स्थिर std::pair<पूर्णांक, MemWr> &a, स्थिर std::pair<पूर्णांक, MemWr> &b) { वापस a.first < b.first; });
			क्रम (स्वतः &it : ports) {
				res.wr_ports.धकेलो_back(it.second);
				wr_portid.धकेलो_back(it.first);
			}
			क्रम (पूर्णांक i = 0; i < Get_आकार(res.wr_ports); i++) {
				स्वतः &port = res.wr_ports[i];
				bool is_compat = port.cell->type == ID($memwr);
				अगर (is_compat) {
					port.priority_mask.resize(Get_आकार(res.wr_ports));
					क्रम (पूर्णांक j = 0; j < i; j++) {
						स्वतः &oport = res.wr_ports[j];
						अगर (port.clk_enable != oport.clk_enable)
							जारी;
						अगर (port.clk_enable && port.clk != oport.clk)
							जारी;
						अगर (port.clk_enable && port.clk_polarity != oport.clk_polarity)
							जारी;
						port.priority_mask[j] = true;
					}
				} अन्यथा {
					Const orig_prio_mask = port.cell->parameters.at(ID::PRIORITY_MASK);
					क्रम (पूर्णांक orig_portid : wr_portid) {
						bool has_prio = orig_portid < Get_आकार(orig_prio_mask) && orig_prio_mask[orig_portid] == State::S1;
						port.priority_mask.धकेलो_back(has_prio);
					}
				}
			}
		}
		अगर (सूचकांक.inits.गिनती(memid)) {
			std::vector<std::pair<पूर्णांक, MemInit>> inits;
			क्रम (स्वतः cell : सूचकांक.inits.at(memid)) {
				MemInit init;
				init.cell = cell;
				init.attributes = cell->attributes;
				स्वतः addr = cell->getPort(ID::ADDR);
				स्वतः तथ्य = cell->getPort(ID::तथ्य_2);
				अगर (!addr.is_fully_const())
					log_error("Non-constant address %s in memory initialization %s.\n", log_signal(addr), cell);
				अगर (!तथ्य.is_fully_const())
					log_error("Non-constant data %s in memory initialization %s.\n", log_signal(तथ्य), cell);
				init.addr = addr.as_const();
				init.तथ्य = तथ्य.as_const();
				अगर (cell->type == ID($meminit_v2)) {
					स्वतः en = cell->getPort(ID::EN);
					अगर (!en.is_fully_const())
						log_error("Non-constant enable %s in memory initialization %s.\n", log_signal(en), cell);
					init.en = en.as_const();
				} अन्यथा {
					init.en = RTLIL::Const(State::S1, mem->width);
				}
				inits.धकेलो_back(std::make_pair(cell->parameters.at(ID::PRIORITY).as_int(), init));
			}
			std::sort(inits.begin(), inits.end(), [](स्थिर std::pair<पूर्णांक, MemInit> &a, स्थिर std::pair<पूर्णांक, MemInit> &b) { वापस a.first < b.first; });
			क्रम (स्वतः &it : inits)
				res.inits.धकेलो_back(it.second);
		}
		क्रम (पूर्णांक i = 0; i < Get_आकार(res.rd_ports); i++) {
			स्वतः &port = res.rd_ports[i];
			bool is_compat = port.cell->type == ID($memrd);
			अगर (is_compat) {
				port.transparency_mask.resize(Get_आकार(res.wr_ports));
				port.collision_x_mask.resize(Get_आकार(res.wr_ports));
				अगर (!rd_transparent[i])
					जारी;
				अगर (!port.clk_enable)
					जारी;
				क्रम (पूर्णांक j = 0; j < Get_आकार(res.wr_ports); j++) {
					स्वतः &wport = res.wr_ports[j];
					अगर (!wport.clk_enable)
						जारी;
					अगर (port.clk != wport.clk)
						जारी;
					अगर (port.clk_polarity != wport.clk_polarity)
						जारी;
					port.transparency_mask[j] = true;
				}
			} अन्यथा {
				Const orig_trans_mask = port.cell->parameters.at(ID::TRANSPARENCY_MASK);
				Const orig_cx_mask = port.cell->parameters.at(ID::COLLISION_X_MASK);
				क्रम (पूर्णांक orig_portid : wr_portid) {
					port.transparency_mask.धकेलो_back(orig_portid < Get_आकार(orig_trans_mask) && orig_trans_mask[orig_portid] == State::S1);
					port.collision_x_mask.धकेलो_back(orig_portid < Get_आकार(orig_cx_mask) && orig_cx_mask[orig_portid] == State::S1);
				}
			}
		}
		res.check();
		वापस res;
	}

	Mem mem_from_cell(Cell *cell) {
		Mem res(cell->module, cell->twines().add(cell->parameters.at(ID::MEMID).decode_string()),
			cell->parameters.at(ID::WIDTH).as_int(),
			cell->parameters.at(ID::OFFSET).as_int(),
			cell->parameters.at(ID::आकार_2).as_int()
		);
		bool is_compat = cell->type == ID($mem);
		पूर्णांक abits = cell->parameters.at(ID::ABITS).as_int();
		res.packed = true;
		res.cell = cell;
		res.attributes = cell->attributes;
		Const &init = cell->parameters.at(ID::INIT);
		अगर (!init.is_fully_undef()) {
			पूर्णांक pos = 0;
			जबतक (pos < res.आकार) {
				Const word = init.extract(pos * res.width, res.width, State::Sx);
				अगर (word.is_fully_undef()) {
					pos++;
				} अन्यथा {
					पूर्णांक epos;
					क्रम (epos = pos; epos < res.आकार; epos++) {
						Const eword = init.extract(epos * res.width, res.width, State::Sx);
						अगर (eword.is_fully_undef())
							अवरोध;
					}
					MemInit minit;
					minit.addr = res.start_offset + pos;
					minit.तथ्य = init.extract(pos * res.width, (epos - pos) * res.width, State::Sx);
					minit.en = RTLIL::Const(State::S1, res.width);
					res.inits.धकेलो_back(minit);
					pos = epos;
				}
			}
		}
		पूर्णांक n_rd_ports = cell->parameters.at(ID::RD_PORTS).as_int();
		पूर्णांक n_wr_ports = cell->parameters.at(ID::WR_PORTS).as_int();
		Const rd_wide_continuation = is_compat ? Const(State::S0, n_rd_ports) : cell->parameters.at(ID::RD_WIDE_CONTINUATION);
		Const wr_wide_continuation = is_compat ? Const(State::S0, n_wr_ports) : cell->parameters.at(ID::WR_WIDE_CONTINUATION);
		क्रम (पूर्णांक i = 0, ni; i < n_rd_ports; i = ni) {
			ni = i + 1;
			जबतक (ni < n_rd_ports && rd_wide_continuation[ni] == State::S1)
				ni++;
			MemRd mrd;
			mrd.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mrd.wide_log2));
			mrd.clk_enable = cell->parameters.at(ID::RD_CLK_ENABLE).extract(i, 1).as_bool();
			mrd.clk_polarity = cell->parameters.at(ID::RD_CLK_POLARITY).extract(i, 1).as_bool();
			mrd.clk = cell->getPort(ID::RD_CLK).extract(i, 1);
			mrd.en = cell->getPort(ID::RD_EN).extract(i, 1);
			mrd.addr = cell->getPort(ID::RD_ADDR).extract(i * abits, abits);
			mrd.तथ्य = cell->getPort(ID::RD_तथ्य).extract(i * res.width, (ni - i) * res.width);
			अगर (is_compat) {
				mrd.ce_over_srst = false;
				mrd.arst_मूल्य = Const(State::Sx, res.width << mrd.wide_log2);
				mrd.srst_मूल्य = Const(State::Sx, res.width << mrd.wide_log2);
				mrd.init_मूल्य = Const(State::Sx, res.width << mrd.wide_log2);
				mrd.arst = State::S0;
				mrd.srst = State::S0;
			} अन्यथा {
				mrd.ce_over_srst = cell->parameters.at(ID::RD_CE_OVER_SRST).extract(i, 1).as_bool();
				mrd.arst_मूल्य = cell->parameters.at(ID::RD_ARST_मूल्य).extract(i * res.width, (ni - i) * res.width);
				mrd.srst_मूल्य = cell->parameters.at(ID::RD_SRST_मूल्य).extract(i * res.width, (ni - i) * res.width);
				mrd.init_मूल्य = cell->parameters.at(ID::RD_INIT_मूल्य).extract(i * res.width, (ni - i) * res.width);
				mrd.arst = cell->getPort(ID::RD_ARST).extract(i, 1);
				mrd.srst = cell->getPort(ID::RD_SRST).extract(i, 1);
			}
			अगर (!is_compat) {
				Const transparency_mask = cell->parameters.at(ID::RD_TRANSPARENCY_MASK).extract(i * n_wr_ports, n_wr_ports);
				Const collision_x_mask = cell->parameters.at(ID::RD_COLLISION_X_MASK).extract(i * n_wr_ports, n_wr_ports);
				क्रम (पूर्णांक j = 0; j < n_wr_ports; j++)
					अगर (wr_wide_continuation[j] != State::S1) {
						mrd.transparency_mask.धकेलो_back(transparency_mask[j] == State::S1);
						mrd.collision_x_mask.धकेलो_back(collision_x_mask[j] == State::S1);
					}
			}
			res.rd_ports.धकेलो_back(mrd);
		}
		क्रम (पूर्णांक i = 0, ni; i < n_wr_ports; i = ni) {
			ni = i + 1;
			जबतक (ni < n_wr_ports && wr_wide_continuation[ni] == State::S1)
				ni++;
			MemWr mwr;
			mwr.wide_log2 = ceil_log2(ni - i);
			log_assert(ni - i == (1 << mwr.wide_log2));
			mwr.clk_enable = cell->parameters.at(ID::WR_CLK_ENABLE).extract(i, 1).as_bool();
			mwr.clk_polarity = cell->parameters.at(ID::WR_CLK_POLARITY).extract(i, 1).as_bool();
			mwr.clk = cell->getPort(ID::WR_CLK).extract(i, 1);
			mwr.en = cell->getPort(ID::WR_EN).extract(i * res.width, (ni - i) * res.width);
			mwr.addr = cell->getPort(ID::WR_ADDR).extract(i * abits, abits);
			mwr.तथ्य = cell->getPort(ID::WR_तथ्य).extract(i * res.width, (ni - i) * res.width);
			अगर (!is_compat) {
				Const priority_mask = cell->parameters.at(ID::WR_PRIORITY_MASK).extract(i * n_wr_ports, n_wr_ports);
				क्रम (पूर्णांक j = 0; j < n_wr_ports; j++)
					अगर (wr_wide_continuation[j] != State::S1)
						mwr.priority_mask.धकेलो_back(priority_mask[j] == State::S1);
			}
			res.wr_ports.धकेलो_back(mwr);
		}
		अगर (is_compat) {
			क्रम (पूर्णांक i = 0; i < Get_आकार(res.wr_ports); i++) {
				स्वतः &port = res.wr_ports[i];
				port.priority_mask.resize(Get_आकार(res.wr_ports));
				क्रम (पूर्णांक j = 0; j < i; j++) {
					स्वतः &oport = res.wr_ports[j];
					अगर (port.clk_enable != oport.clk_enable)
						जारी;
					अगर (port.clk_enable && port.clk != oport.clk)
						जारी;
					अगर (port.clk_enable && port.clk_polarity != oport.clk_polarity)
						जारी;
					port.priority_mask[j] = true;
				}
			}
			क्रम (पूर्णांक i = 0; i < Get_आकार(res.rd_ports); i++) {
				स्वतः &port = res.rd_ports[i];
				port.transparency_mask.resize(Get_आकार(res.wr_ports));
				port.collision_x_mask.resize(Get_आकार(res.wr_ports));
				अगर (!cell->parameters.at(ID::RD_TRANSPARENT).extract(i, 1).as_bool())
					जारी;
				अगर (!port.clk_enable)
					जारी;
				क्रम (पूर्णांक j = 0; j < Get_आकार(res.wr_ports); j++) {
					स्वतः &wport = res.wr_ports[j];
					अगर (!wport.clk_enable)
						जारी;
					अगर (port.clk != wport.clk)
						जारी;
					अगर (port.clk_polarity != wport.clk_polarity)
						जारी;
					port.transparency_mask[j] = true;
				}
			}
		}
		res.check();
		वापस res;
	}

}

std::vector<Mem> Mem::get_all_memories(Module *module) {
	std::vector<Mem> res;
	Mem_सूचकांक सूचकांक(module);
	क्रम (स्वतः it: module->memories) {
		res.धकेलो_back(mem_from_memory(module, it.second, सूचकांक));
	}
	क्रम (स्वतः cell: module->cells()) {
		अगर (cell->type.in(ID($mem), ID($mem_v2)))
			res.धकेलो_back(mem_from_cell(cell));
	}
	वापस res;
}

std::vector<Mem> Mem::get_selected_memories(Module *module) {
	std::vector<Mem> res;
	Mem_सूचकांक सूचकांक(module);
	क्रम (स्वतः it: module->memories) {
		अगर (module->design->selected(module, it.second))
			res.धकेलो_back(mem_from_memory(module, it.second, सूचकांक));
	}
	क्रम (स्वतः cell: module->selected_cells()) {
		अगर (cell->type.in(ID($mem), ID($mem_v2)))
			res.धकेलो_back(mem_from_cell(cell));
	}
	वापस res;
}

Cell *Mem::extract_rdff(पूर्णांक idx, FfInitVals *initvals) {
	MemRd &port = rd_ports[idx];

	अगर (!port.clk_enable)
		वापस nullptr;

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
	bool trans_use_addr = true;
	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++)
		अगर (!port.transparency_mask[i] && !wr_ports[i].removed)
			trans_use_addr = false;

	// If there are no write ports at all, we could possibly use either way; do data
	// FF in this case.
	अगर (Get_आकार(wr_ports) == 0)
		trans_use_addr = false;

	अगर (port.en != State::S1 || port.srst != State::S0 || port.arst != State::S0 || !port.init_मूल्य.is_fully_undef())
		trans_use_addr = false;

	अगर (trans_use_addr)
	{
		// Do not put a register in front of constant address bits — this is both
		// unnecessary and will break wide ports.
		पूर्णांक width = 0;
		क्रम (पूर्णांक i = 0; i < Get_आकार(port.addr); i++)
			अगर (port.addr[i].wire)
				width++;

		अगर (width)
		{
			SigSpec sig_q = module->addWire(stringf("$%s$rdreg[%d]$q", memid_str, idx), width);
			SigSpec sig_d;

			पूर्णांक pos = 0;
			क्रम (पूर्णांक i = 0; i < Get_आकार(port.addr); i++)
				अगर (port.addr[i].wire) {
					sig_d.append(port.addr[i]);
					port.addr[i] = sig_q[pos++];
				}

			c = module->addDff(stringf("$%s$rdreg[%d]", memid_str, idx), port.clk, sig_d, sig_q, port.clk_polarity);
		} अन्यथा {
			c = nullptr;
		}
	}
	अन्यथा
	{
		log_assert(port.arst == State::S0 || port.srst == State::S0);

		SigSpec async_d = module->addWire(stringf("$%s$rdreg[%d]$d", memid_str, idx), Get_आकार(port.तथ्य));
		SigSpec sig_d = async_d;

		क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++) {
			स्वतः &wport = wr_ports[i];
			अगर (wport.removed)
				जारी;
			अगर (port.transparency_mask[i] || port.collision_x_mask[i]) {
				log_assert(wport.clk_enable);
				log_assert(wport.clk == port.clk);
				log_assert(wport.clk_enable == port.clk_enable);
				पूर्णांक min_wide_log2 = std::min(port.wide_log2, wport.wide_log2);
				पूर्णांक max_wide_log2 = std::max(port.wide_log2, wport.wide_log2);
				bool wide_write = wport.wide_log2 > port.wide_log2;
				क्रम (पूर्णांक sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
					SigSpec raddr = port.addr;
					SigSpec waddr = wport.addr;
					अगर (wide_write)
						waddr = wport.sub_addr(sub);
					अन्यथा
						raddr = port.sub_addr(sub);
					SigSpec addr_eq;
					अगर (raddr != waddr)
						addr_eq = module->Eq(stringf("$%s$rdtransen[%d][%d][%d]$d", memid_str, idx, i, sub), raddr, waddr);
					पूर्णांक pos = 0;
					पूर्णांक ewidth = width << min_wide_log2;
					पूर्णांक wsub = wide_write ? sub : 0;
					पूर्णांक rsub = wide_write ? 0 : sub;
					जबतक (pos < ewidth) {
						पूर्णांक epos = pos;
						जबतक (epos < ewidth && wport.en[epos + wsub * width] == wport.en[pos + wsub * width])
							epos++;
						SigSpec cur = sig_d.extract(pos + rsub * width, epos-pos);
						SigSpec other = port.transparency_mask[i] ? wport.तथ्य.extract(pos + wsub * width, epos-pos) : Const(State::Sx, epos-pos);
						SigSpec cond;
						अगर (raddr != waddr)
							cond = module->And(stringf("$%s$rdtransgate[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), wport.en[pos + wsub * width], addr_eq);
						अन्यथा
							cond = wport.en[pos + wsub * width];
						SigSpec merged = module->Mux(stringf("$%s$rdtransmux[%d][%d][%d][%d]$d", memid_str, idx, i, sub, pos), cur, other, cond);
						sig_d.replace(pos + rsub * width, merged);
						pos = epos;
					}
				}
			}
		}

		IdString name = module->twines().add(stringf("$%s$rdreg[%d]", memid_str, idx));
		Ff_तथ्य ff(module, initvals, name);
		ff.width = Get_आकार(port.तथ्य);
		ff.has_clk = true;
		ff.sig_clk = port.clk;
		ff.pol_clk = port.clk_polarity;
		अगर (port.en != State::S1) {
			ff.has_ce = true;
			ff.pol_ce = true;
			ff.sig_ce = port.en;
		}
		अगर (port.arst != State::S0) {
			ff.has_arst = true;
			ff.pol_arst = true;
			ff.sig_arst = port.arst;
			ff.val_arst = port.arst_मूल्य;
		}
		अगर (port.srst != State::S0) {
			ff.has_srst = true;
			ff.pol_srst = true;
			ff.sig_srst = port.srst;
			ff.val_srst = port.srst_मूल्य;
			ff.ce_over_srst = ff.has_ce && port.ce_over_srst;
		}
		ff.sig_d = sig_d;
		ff.sig_q = port.तथ्य;
		ff.val_init = port.init_मूल्य;
		port.तथ्य = async_d;
		c = ff.emit();
	}

	अगर (c)
		log("Extracted %s FF from read port %d of %s.%s: %s\n", trans_use_addr ? "addr" : "data",
			idx, module, memid.unescape(), c);

	port.en = State::S1;
	port.clk = State::S0;
	port.arst = State::S0;
	port.srst = State::S0;
	port.clk_enable = false;
	port.clk_polarity = true;
	port.ce_over_srst = false;
	port.arst_मूल्य = Const(State::Sx, Get_आकार(port.तथ्य));
	port.srst_मूल्य = Const(State::Sx, Get_आकार(port.तथ्य));
	port.init_मूल्य = Const(State::Sx, Get_आकार(port.तथ्य));

	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++) {
		port.transparency_mask[i] = false;
		port.collision_x_mask[i] = false;
	}

	वापस c;
}

व्योम Mem::narrow() {
	// NOTE: several passes depend on this function not modifying
	// the design at all until (and unless) emit() is called.
	// Be careful to preserve this.
	std::vector<MemRd> new_rd_ports;
	std::vector<MemWr> new_wr_ports;
	std::vector<std::pair<पूर्णांक, पूर्णांक>> new_rd_map;
	std::vector<std::pair<पूर्णांक, पूर्णांक>> new_wr_map;
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_ports); i++) {
		स्वतः &port = rd_ports[i];
		क्रम (पूर्णांक sub = 0; sub < (1 << port.wide_log2); sub++) {
			new_rd_map.धकेलो_back(std::make_pair(i, sub));
		}
	}
	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++) {
		स्वतः &port = wr_ports[i];
		क्रम (पूर्णांक sub = 0; sub < (1 << port.wide_log2); sub++) {
			new_wr_map.धकेलो_back(std::make_pair(i, sub));
		}
	}
	क्रम (स्वतः &it : new_rd_map) {
		MemRd &orig = rd_ports[it.first];
		MemRd port = orig;
		अगर (it.second != 0)
			port.cell = nullptr;
		अगर (port.wide_log2) {
			port.तथ्य = port.तथ्य.extract(it.second * width, width);
			port.init_मूल्य = port.init_मूल्य.extract(it.second * width, width);
			port.arst_मूल्य = port.arst_मूल्य.extract(it.second * width, width);
			port.srst_मूल्य = port.srst_मूल्य.extract(it.second * width, width);
			port.addr = port.sub_addr(it.second);
			port.wide_log2 = 0;
		}
		port.transparency_mask.सफा();
		port.collision_x_mask.सफा();
		क्रम (स्वतः &it2 : new_wr_map)
			port.transparency_mask.धकेलो_back(orig.transparency_mask[it2.first]);
		क्रम (स्वतः &it2 : new_wr_map)
			port.collision_x_mask.धकेलो_back(orig.collision_x_mask[it2.first]);
		new_rd_ports.धकेलो_back(port);
	}
	क्रम (स्वतः &it : new_wr_map) {
		MemWr &orig = wr_ports[it.first];
		MemWr port = orig;
		अगर (it.second != 0)
			port.cell = nullptr;
		अगर (port.wide_log2) {
			port.तथ्य = port.तथ्य.extract(it.second * width, width);
			port.en = port.en.extract(it.second * width, width);
			port.addr = port.sub_addr(it.second);
			port.wide_log2 = 0;
		}
		port.priority_mask.सफा();
		क्रम (स्वतः &it2 : new_wr_map)
			port.priority_mask.धकेलो_back(orig.priority_mask[it2.first]);
		new_wr_ports.धकेलो_back(port);
	}
	std::swap(rd_ports, new_rd_ports);
	std::swap(wr_ports, new_wr_ports);
}

व्योम Mem::emulate_priority(पूर्णांक idx1, पूर्णांक idx2, FfInitVals *initvals)
{
	स्वतः &port1 = wr_ports[idx1];
	स्वतः &port2 = wr_ports[idx2];
	अगर (!port2.priority_mask[idx1])
		वापस;
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_ports); i++) {
		स्वतः &rport = rd_ports[i];
		अगर (rport.removed)
			जारी;
		अगर (rport.transparency_mask[idx1] && !(rport.transparency_mask[idx2] || rport.collision_x_mask[idx2]))
			emulate_transparency(idx1, i, initvals);
	}
	पूर्णांक min_wide_log2 = std::min(port1.wide_log2, port2.wide_log2);
	पूर्णांक max_wide_log2 = std::max(port1.wide_log2, port2.wide_log2);
	bool wide1 = port1.wide_log2 > port2.wide_log2;
	क्रम (पूर्णांक sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec addr1 = port1.addr;
		SigSpec addr2 = port2.addr;
		अगर (wide1)
			addr1 = port1.sub_addr(sub);
		अन्यथा
			addr2 = port2.sub_addr(sub);
		SigSpec addr_eq = module->Eq(NEW_ID, addr1, addr2);
		पूर्णांक ewidth = width << min_wide_log2;
		पूर्णांक sub1 = wide1 ? sub : 0;
		पूर्णांक sub2 = wide1 ? 0 : sub;
		dict<std::pair<SigBit, SigBit>, SigBit> cache;
		क्रम (पूर्णांक pos = 0; pos < ewidth; pos++) {
			SigBit &en1 = port1.en[pos + sub1 * width];
			SigBit &en2 = port2.en[pos + sub2 * width];
			std::pair<SigBit, SigBit> कुंजी(en1, en2);
			अगर (cache.गिनती(कुंजी)) {
				en1 = cache[कुंजी];
			} अन्यथा {
				SigBit active2 = module->And(NEW_ID, addr_eq, en2);
				SigBit nactive2 = module->Not(NEW_ID, active2);
				en1 = cache[कुंजी] = module->And(NEW_ID, en1, nactive2);
			}
		}
	}
	port2.priority_mask[idx1] = false;
}

व्योम Mem::emulate_transparency(पूर्णांक widx, पूर्णांक ridx, FfInitVals *initvals) {
	स्वतः &wport = wr_ports[widx];
	स्वतः &rport = rd_ports[ridx];
	log_assert(rport.transparency_mask[widx]);
	// If other write ports have priority over this one, emulate their transparency too.
	क्रम (पूर्णांक i = Get_आकार(wr_ports) - 1; i > widx; i--) {
		अगर (wr_ports[i].removed)
			जारी;
		अगर (rport.transparency_mask[i] && wr_ports[i].priority_mask[widx])
			emulate_transparency(i, ridx, initvals);
	}
	पूर्णांक min_wide_log2 = std::min(rport.wide_log2, wport.wide_log2);
	पूर्णांक max_wide_log2 = std::max(rport.wide_log2, wport.wide_log2);
	bool wide_write = wport.wide_log2 > rport.wide_log2;
	// The write data FF doesn't need full reset/init behavior, as it'll be masked by
	// the mux whenever this would be relevant.  It does, however, need to have the same
	// clock enable signal as the read port.
	SigSpec wdata_q = module->addWire(NEW_ID, Get_आकार(wport.तथ्य));
	module->addDffe(NEW_ID, rport.clk, rport.en, wport.तथ्य, wdata_q, rport.clk_polarity, true);
	क्रम (पूर्णांक sub = 0; sub < (1 << max_wide_log2); sub += (1 << min_wide_log2)) {
		SigSpec raddr = rport.addr;
		SigSpec waddr = wport.addr;
		क्रम (पूर्णांक j = min_wide_log2; j < max_wide_log2; j++)
			अगर (wide_write)
				waddr = wport.sub_addr(sub);
			अन्यथा
				raddr = rport.sub_addr(sub);
		SigSpec addr_eq;
		अगर (raddr != waddr)
			addr_eq = module->Eq(NEW_ID, raddr, waddr);
		पूर्णांक pos = 0;
		पूर्णांक ewidth = width << min_wide_log2;
		पूर्णांक wsub = wide_write ? sub : 0;
		पूर्णांक rsub = wide_write ? 0 : sub;
		SigSpec rdata_a = module->addWire(NEW_ID, ewidth);
		जबतक (pos < ewidth) {
			पूर्णांक epos = pos;
			जबतक (epos < ewidth && wport.en[epos + wsub * width] == wport.en[pos + wsub * width])
				epos++;
			SigSpec cond;
			अगर (raddr != waddr)
				cond = module->And(NEW_ID, wport.en[pos + wsub * width], addr_eq);
			अन्यथा
				cond = wport.en[pos + wsub * width];
			SigSpec cond_q = module->addWire(NEW_ID);
			// The FF for storing the bypass enable signal must be carefully
			// constructed to preserve the overall init/reset/enable behavior
			// of the whole port.
			Ff_तथ्य ff(module, initvals, NEW_ID);
			ff.width = 1;
			ff.sig_q = cond_q;
			ff.sig_d = cond;
			ff.has_clk = true;
			ff.sig_clk = rport.clk;
			ff.pol_clk = rport.clk_polarity;
			अगर (rport.en != State::S1) {
				ff.has_ce = true;
				ff.sig_ce = rport.en;
				ff.pol_ce = true;
			}
			अगर (rport.arst != State::S0) {
				ff.has_arst = true;
				ff.sig_arst = rport.arst;
				ff.pol_arst = true;
				ff.val_arst = State::S0;
			}
			अगर (rport.srst != State::S0) {
				ff.has_srst = true;
				ff.sig_srst = rport.srst;
				ff.pol_srst = true;
				ff.val_srst = State::S0;
				ff.ce_over_srst = rport.ce_over_srst;
			}
			अगर (!rport.init_मूल्य.is_fully_undef())
				ff.val_init = State::S0;
			अन्यथा
				ff.val_init = State::Sx;
			ff.emit();
			// And the final bypass mux.
			SigSpec cur = rdata_a.extract(pos, epos-pos);
			SigSpec other = wdata_q.extract(pos + wsub * width, epos-pos);
			SigSpec dest = rport.तथ्य.extract(pos + rsub * width, epos-pos);
			module->addMux(NEW_ID, cur, other, cond_q, dest);
			pos = epos;
		}
		rport.तथ्य.replace(rsub * width, rdata_a);
	}
	rport.transparency_mask[widx] = false;
	rport.collision_x_mask[widx] = true;
}

व्योम Mem::prepare_wr_merge(पूर्णांक idx1, पूर्णांक idx2, FfInitVals *initvals) {
	log_assert(idx1 < idx2);
	स्वतः &port1 = wr_ports[idx1];
	स्वतः &port2 = wr_ports[idx2];
	// If port 2 has priority over a port before port 1, make port 1 have priority too.
	क्रम (पूर्णांक i = 0; i < idx1; i++)
		अगर (port2.priority_mask[i])
			port1.priority_mask[i] = true;
	// If port 2 has priority over a port after port 1, emulate it.
	क्रम (पूर्णांक i = idx1 + 1; i < idx2; i++)
		अगर (port2.priority_mask[i] && !wr_ports[i].removed)
			emulate_priority(i, idx2, initvals);
	// If some port had priority over port 2, make it have priority over the merged port too.
	क्रम (पूर्णांक i = idx2 + 1; i < Get_आकार(wr_ports); i++) {
		स्वतः &oport = wr_ports[i];
		अगर (oport.priority_mask[idx2])
			oport.priority_mask[idx1] = true;
	}
	// Make sure all read ports have identical collision/transparency behavior wrt both
	// ports.
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_ports); i++) {
		स्वतः &rport = rd_ports[i];
		अगर (rport.removed)
			जारी;
		// If collision already undefined with both ports, it's fine.
		अगर (rport.collision_x_mask[idx1] && rport.collision_x_mask[idx2])
			जारी;
		// If one port has undefined collision, change it to the behavior
		// of the other port.
		अगर (rport.collision_x_mask[idx1]) {
			rport.collision_x_mask[idx1] = false;
			rport.transparency_mask[idx1] = rport.transparency_mask[idx2];
			जारी;
		}
		अगर (rport.collision_x_mask[idx2]) {
			rport.collision_x_mask[idx2] = false;
			rport.transparency_mask[idx2] = rport.transparency_mask[idx1];
			जारी;
		}
		// If transparent with both ports, also fine.
		अगर (rport.transparency_mask[idx1] && rport.transparency_mask[idx2])
			जारी;
		// If transparent with only one, emulate it, and remove the collision-X
		// flag that emulate_transparency will set (to align with the other port).
		अगर (rport.transparency_mask[idx1]) {
			emulate_transparency(idx1, i, initvals);
			rport.collision_x_mask[idx1] = false;
			जारी;
		}
		अगर (rport.transparency_mask[idx2]) {
			emulate_transparency(idx2, i, initvals);
			rport.collision_x_mask[idx2] = false;
			जारी;
		}
		// If we got here, it's transparent with neither port, which is fine.
	}
}

व्योम Mem::prepare_rd_merge(पूर्णांक idx1, पूर्णांक idx2, FfInitVals *initvals) {
	स्वतः &port1 = rd_ports[idx1];
	स्वतः &port2 = rd_ports[idx2];
	// Note that going through write ports in order is important, since
	// emulating transparency of a write port can change transparency
	// mask for higher-numbered ports (due to transitive transparency
	// emulation needed because of write port priority).
	क्रम (पूर्णांक i = 0; i < Get_आकार(wr_ports); i++) {
		अगर (wr_ports[i].removed)
			जारी;
		// Both ports undefined, OK.
		अगर (port1.collision_x_mask[i] && port2.collision_x_mask[i])
			जारी;
		// Only one port undefined — change its behavior
		// to align with the other port.
		अगर (port1.collision_x_mask[i]) {
			port1.collision_x_mask[i] = false;
			port1.transparency_mask[i] = port2.transparency_mask[i];
			जारी;
		}
		अगर (port2.collision_x_mask[i]) {
			port2.collision_x_mask[i] = false;
			port2.transparency_mask[i] = port1.transparency_mask[i];
			जारी;
		}
		// Both ports transparent, OK.
		अगर (port1.transparency_mask[i] && port2.transparency_mask[i])
			जारी;
		// Only one port transparent — emulate transparency
		// on the other.
		अगर (port1.transparency_mask[i]) {
			emulate_transparency(i, idx1, initvals);
			port1.collision_x_mask[i] = false;
			जारी;
		}
		अगर (port2.transparency_mask[i]) {
			emulate_transparency(i, idx2, initvals);
			port2.collision_x_mask[i] = false;
			जारी;
		}
		// No ports transparent, OK.
	}

}

व्योम Mem::widen_prep(पूर्णांक wide_log2) {
	// Make sure start_offset and size are aligned to the port width,
	// adjust if necessary.
	पूर्णांक mask = ((1 << wide_log2) - 1);
	पूर्णांक delta = start_offset & mask;
	start_offset -= delta;
	आकार += delta;
	अगर (आकार & mask) {
		आकार |= mask;
		आकार++;
	}
}

व्योम Mem::widen_wr_port(पूर्णांक idx, पूर्णांक wide_log2) {
	widen_prep(wide_log2);
	स्वतः &port = wr_ports[idx];
	log_assert(port.wide_log2 <= wide_log2);
	अगर (port.wide_log2 < wide_log2) {
		SigSpec new_तथ्य, new_en;
		SigSpec addr_lo = port.addr.extract(0, wide_log2);
		क्रम (पूर्णांक sub = 0; sub < (1 << wide_log2); sub += (1 << port.wide_log2))
		{
			Const cur_addr_lo(sub, wide_log2);
			अगर (addr_lo == cur_addr_lo) {
				// Always writes to this subword.
				new_तथ्य.append(port.तथ्य);
				new_en.append(port.en);
			} अन्यथा अगर (addr_lo.is_fully_const()) {
				// Never writes to this subword.
				new_तथ्य.append(Const(State::Sx, Get_आकार(port.तथ्य)));
				new_en.append(Const(State::S0, Get_आकार(port.तथ्य)));
			} अन्यथा {
				// May or may not write to this subword.
				new_तथ्य.append(port.तथ्य);
				SigSpec addr_eq = module->Eq(NEW_ID, addr_lo, cur_addr_lo);
				SigSpec en = module->Mux(NEW_ID, Const(State::S0, Get_आकार(port.तथ्य)), port.en, addr_eq);
				new_en.append(en);
			}
		}
		port.addr.replace(port.wide_log2, Const(State::S0, wide_log2 - port.wide_log2));
		port.तथ्य = new_तथ्य;
		port.en = new_en;
		port.wide_log2 = wide_log2;
	}
}

व्योम Mem::emulate_rden(पूर्णांक idx, FfInitVals *initvals) {
	स्वतः &port = rd_ports[idx];
	log_assert(port.clk_enable);
	emulate_rd_ce_over_srst(idx);
	Wire *new_तथ्य = module->addWire(NEW_ID, Get_आकार(port.तथ्य));
	Wire *पूर्व_तथ्य = module->addWire(NEW_ID, Get_आकार(port.तथ्य));
	Wire *sel = module->addWire(NEW_ID);
	Ff_तथ्य ff_sel(module, initvals, NEW_ID);
	Ff_तथ्य ff_तथ्य(module, initvals, NEW_ID);
	ff_sel.width = 1;
	ff_sel.has_clk = true;
	ff_sel.sig_clk = port.clk;
	ff_sel.pol_clk = port.clk_polarity;
	ff_sel.sig_d = port.en;
	ff_sel.sig_q = sel;
	ff_तथ्य.width = Get_आकार(port.तथ्य);
	ff_तथ्य.has_clk = true;
	ff_तथ्य.sig_clk = port.clk;
	ff_तथ्य.pol_clk = port.clk_polarity;
	ff_तथ्य.sig_d = port.तथ्य;
	ff_तथ्य.sig_q = पूर्व_तथ्य;
	अगर (!port.init_मूल्य.is_fully_undef()) {
		ff_sel.val_init = State::S0;
		ff_तथ्य.val_init = port.init_मूल्य;
		port.init_मूल्य = Const(State::Sx, Get_आकार(port.तथ्य));
	} अन्यथा {
		ff_sel.val_init = State::Sx;
		ff_तथ्य.val_init = Const(State::Sx, Get_आकार(port.तथ्य));
	}
	अगर (port.arst != State::S0) {
		ff_sel.has_arst = true;
		ff_sel.val_arst = State::S0;
		ff_sel.sig_arst = port.arst;
		ff_sel.pol_arst = true;
		ff_तथ्य.has_arst = true;
		ff_तथ्य.val_arst = port.arst_मूल्य;
		ff_तथ्य.sig_arst = port.arst;
		ff_तथ्य.pol_arst = true;
		port.arst = State::S0;
	}
	अगर (port.srst != State::S0) {
		log_assert(!port.ce_over_srst);
		ff_sel.has_srst = true;
		ff_sel.val_srst = State::S0;
		ff_sel.sig_srst = port.srst;
		ff_sel.pol_srst = true;
		ff_sel.ce_over_srst = false;
		ff_तथ्य.has_srst = true;
		ff_तथ्य.val_srst = port.srst_मूल्य;
		ff_तथ्य.sig_srst = port.srst;
		ff_तथ्य.pol_srst = true;
		ff_तथ्य.ce_over_srst = false;
		port.srst = State::S0;
	}
	ff_sel.emit();
	ff_तथ्य.emit();
	module->addMux(NEW_ID, पूर्व_तथ्य, new_तथ्य, sel, port.तथ्य);
	port.तथ्य = new_तथ्य;
	port.en = State::S1;
}

व्योम Mem::emulate_reset(पूर्णांक idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals) {
	स्वतः &port = rd_ports[idx];
	अगर (emu_init && !port.init_मूल्य.is_fully_undef()) {
		Wire *sel = module->addWire(NEW_ID);
		Ff_तथ्य ff_sel(module, initvals, NEW_ID);
		Wire *new_तथ्य = module->addWire(NEW_ID, Get_आकार(port.तथ्य));
		ff_sel.width = 1;
		ff_sel.has_clk = true;
		ff_sel.sig_clk = port.clk;
		ff_sel.pol_clk = port.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		ff_sel.val_init = State::S0;
		अगर (port.en != State::S1) {
			ff_sel.has_ce = true;
			ff_sel.sig_ce = port.en;
			ff_sel.pol_ce = true;
			ff_sel.ce_over_srst = port.ce_over_srst;
		}
		अगर (port.arst != State::S0) {
			ff_sel.has_arst = true;
			ff_sel.sig_arst = port.arst;
			ff_sel.pol_arst = true;
			अगर (emu_arst && port.arst_मूल्य == port.init_मूल्य) {
				// If we're going to emulate async reset anyway, and the reset
				// value is the same as init value, reuse the same mux.
				ff_sel.val_arst = State::S0;
				port.arst = State::S0;
			} अन्यथा {
				ff_sel.val_arst = State::S1;
			}
		}
		अगर (port.srst != State::S0) {
			ff_sel.has_srst = true;
			ff_sel.sig_srst = port.srst;
			ff_sel.pol_srst = true;
			अगर (emu_srst && port.srst_मूल्य == port.init_मूल्य) {
				ff_sel.val_srst = State::S0;
				port.srst = State::S0;
			} अन्यथा {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->addMux(NEW_ID, port.init_मूल्य, new_तथ्य, sel, port.तथ्य);
		port.तथ्य = new_तथ्य;
		port.init_मूल्य = Const(State::Sx, Get_आकार(port.तथ्य));
	}
	अगर (emu_arst && port.arst != State::S0) {
		Wire *sel = module->addWire(NEW_ID);
		Ff_तथ्य ff_sel(module, initvals, NEW_ID);
		Wire *new_तथ्य = module->addWire(NEW_ID, Get_आकार(port.तथ्य));
		ff_sel.width = 1;
		ff_sel.has_clk = true;
		ff_sel.sig_clk = port.clk;
		ff_sel.pol_clk = port.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		अगर (port.init_मूल्य.is_fully_undef())
			ff_sel.val_init = State::Sx;
		अन्यथा
			ff_sel.val_init = State::S1;
		अगर (port.en != State::S1) {
			ff_sel.has_ce = true;
			ff_sel.sig_ce = port.en;
			ff_sel.pol_ce = true;
			ff_sel.ce_over_srst = port.ce_over_srst;
		}
		ff_sel.has_arst = true;
		ff_sel.sig_arst = port.arst;
		ff_sel.pol_arst = true;
		ff_sel.val_arst = State::S0;
		अगर (port.srst != State::S0) {
			ff_sel.has_srst = true;
			ff_sel.sig_srst = port.srst;
			ff_sel.pol_srst = true;
			अगर (emu_srst && port.srst_मूल्य == port.arst_मूल्य) {
				ff_sel.val_srst = State::S0;
				port.srst = State::S0;
			} अन्यथा {
				ff_sel.val_srst = State::S1;
			}
		}
		ff_sel.emit();
		module->addMux(NEW_ID, port.arst_मूल्य, new_तथ्य, sel, port.तथ्य);
		port.तथ्य = new_तथ्य;
		port.arst = State::S0;
	}
	अगर (emu_srst && port.srst != State::S0) {
		Wire *sel = module->addWire(NEW_ID);
		Ff_तथ्य ff_sel(module, initvals, NEW_ID);
		Wire *new_तथ्य = module->addWire(NEW_ID, Get_आकार(port.तथ्य));
		ff_sel.width = 1;
		ff_sel.has_clk = true;
		ff_sel.sig_clk = port.clk;
		ff_sel.pol_clk = port.clk_polarity;
		ff_sel.sig_d = State::S1;
		ff_sel.sig_q = sel;
		अगर (port.init_मूल्य.is_fully_undef())
			ff_sel.val_init = State::Sx;
		अन्यथा
			ff_sel.val_init = State::S1;
		अगर (port.en != State::S1) {
			ff_sel.has_ce = true;
			ff_sel.sig_ce = port.en;
			ff_sel.pol_ce = true;
			ff_sel.ce_over_srst = port.ce_over_srst;
		}
		ff_sel.has_srst = true;
		ff_sel.sig_srst = port.srst;
		ff_sel.pol_srst = true;
		ff_sel.val_srst = State::S0;
		अगर (port.arst != State::S0) {
			ff_sel.has_arst = true;
			ff_sel.sig_arst = port.arst;
			ff_sel.pol_arst = true;
			ff_sel.val_arst = State::S1;
		}
		ff_sel.emit();
		module->addMux(NEW_ID, port.srst_मूल्य, new_तथ्य, sel, port.तथ्य);
		port.तथ्य = new_तथ्य;
		port.srst = State::S0;
	}
}

व्योम Mem::emulate_rd_ce_over_srst(पूर्णांक idx) {
	स्वतः &port = rd_ports[idx];
	log_assert(port.clk_enable);
	अगर (port.en == State::S1 || port.srst == State::S0 || !port.ce_over_srst) {
		port.ce_over_srst = false;
		वापस;
	}
	port.ce_over_srst = false;
	port.srst = module->And(NEW_ID, port.en, port.srst);
}

व्योम Mem::emulate_rd_srst_over_ce(पूर्णांक idx) {
	स्वतः &port = rd_ports[idx];
	log_assert(port.clk_enable);
	अगर (port.en == State::S1 || port.srst == State::S0 || port.ce_over_srst) {
		port.ce_over_srst = true;
		वापस;
	}
	port.ce_over_srst = true;
	port.en = module->Or(NEW_ID, port.en, port.srst);
}

bool Mem::emulate_read_first_ok() {
	अगर (wr_ports.empty())
		वापस false;
	SigSpec clk = wr_ports[0].clk;
	bool clk_polarity = wr_ports[0].clk_polarity;
	क्रम (स्वतः &port: wr_ports) {
		अगर (!port.clk_enable)
			वापस false;
		अगर (port.clk != clk)
			वापस false;
		अगर (port.clk_polarity != clk_polarity)
			वापस false;
	}
	bool found_read_first = false;
	क्रम (स्वतः &port: rd_ports) {
		अगर (!port.clk_enable)
			वापस false;
		अगर (port.clk != clk)
			वापस false;
		अगर (port.clk_polarity != clk_polarity)
			वापस false;
		// No point doing this operation if there is no read-first relationship
		// in the first place.
		क्रम (पूर्णांक j = 0; j < Get_आकार(wr_ports); j++)
			अगर (!port.transparency_mask[j] && !port.collision_x_mask[j])
				found_read_first = true;
	}
	वापस found_read_first;
}

व्योम Mem::emulate_read_first(FfInitVals *initvals) {
	log_assert(emulate_read_first_ok());
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_ports); i++)
		क्रम (पूर्णांक j = 0; j < Get_आकार(wr_ports); j++)
			अगर (rd_ports[i].transparency_mask[j])
				emulate_transparency(j, i, initvals);
	क्रम (पूर्णांक i = 0; i < Get_आकार(rd_ports); i++)
		क्रम (पूर्णांक j = 0; j < Get_आकार(wr_ports); j++) {
			log_assert(!rd_ports[i].transparency_mask[j]);
			rd_ports[i].collision_x_mask[j] = false;
			rd_ports[i].transparency_mask[j] = true;
		}
	क्रम (स्वतः &port: wr_ports) {
		Wire *new_तथ्य = module->addWire(NEW_ID, Get_आकार(port.तथ्य));
		Wire *new_addr = module->addWire(NEW_ID, Get_आकार(port.addr));
		स्वतः compressed = port.compress_en();
		Wire *new_en = module->addWire(NEW_ID, Get_आकार(compressed.first));
		Ff_तथ्य ff_तथ्य(module, initvals, NEW_ID);
		Ff_तथ्य ff_addr(module, initvals, NEW_ID);
		Ff_तथ्य ff_en(module, initvals, NEW_ID);
		ff_तथ्य.width = Get_आकार(port.तथ्य);
		ff_तथ्य.has_clk = true;
		ff_तथ्य.sig_clk = port.clk;
		ff_तथ्य.pol_clk = port.clk_polarity;
		ff_तथ्य.sig_d = port.तथ्य;
		ff_तथ्य.sig_q = new_तथ्य;;
		ff_तथ्य.val_init = Const(State::Sx, ff_तथ्य.width);
		ff_तथ्य.emit();
		ff_addr.width = Get_आकार(port.addr);
		ff_addr.has_clk = true;
		ff_addr.sig_clk = port.clk;
		ff_addr.pol_clk = port.clk_polarity;
		ff_addr.sig_d = port.addr;
		ff_addr.sig_q = new_addr;;
		ff_addr.val_init = Const(State::Sx, ff_addr.width);
		ff_addr.emit();
		ff_en.width = Get_आकार(compressed.first);
		ff_en.has_clk = true;
		ff_en.sig_clk = port.clk;
		ff_en.pol_clk = port.clk_polarity;
		ff_en.sig_d = compressed.first;
		ff_en.sig_q = new_en;;
		अगर (inits.empty())
			ff_en.val_init = Const(State::Sx, ff_en.width);
		अन्यथा
			ff_en.val_init = Const(State::S0, ff_en.width);
		ff_en.emit();
		port.तथ्य = new_तथ्य;
		port.addr = new_addr;
		port.en = port.decompress_en(compressed.second, new_en);
	}
}

std::pair<SigSpec, std::vector<पूर्णांक>> MemWr::compress_en() {
	SigSpec sig = en[0];
	std::vector<पूर्णांक> swizzle;
	SigBit पूर्व_bit = en[0];
	पूर्णांक idx = 0;
	क्रम (स्वतः &bit: en) {
		अगर (bit != पूर्व_bit) {
			sig.append(bit);
			पूर्व_bit = bit;
			idx++;
		}
		swizzle.धकेलो_back(idx);
	}
	log_assert(idx + 1 == Get_आकार(sig));
	वापस {sig, swizzle};
}

SigSpec MemWr::decompress_en(स्थिर std::vector<पूर्णांक> &swizzle, SigSpec sig) {
	SigSpec res;
	क्रम (पूर्णांक i: swizzle)
		res.append(sig[i]);
	वापस res;
}

MemContents::MemContents(Mem *mem) :
	MemContents(ceil_log2(mem->आकार), mem->width)
{
	क्रम(स्थिर स्वतः &init : mem->inits) {
		अगर(init.en.is_fully_zero()) जारी;
		log_assert(init.en.आकार() == तथ्य_width_2);
		अगर(init.en.is_fully_ones())
			जोड़ो_concatenated(init.addr.as_int(), init.तथ्य);
		अन्यथा {
			// TODO: this case could be handled more efficiently by adding
			// a flag to reserve_range that tells it to preserve
			// previous contents
			addr_t addr = init.addr.as_int();
			addr_t शब्दगण_2 = init.तथ्य.आकार() / तथ्य_width_2;
			RTLIL::Const तथ्य = init.तथ्य;
			log_assert(तथ्य.आकार() % तथ्य_width_2 == 0);
			क्रम(addr_t i = 0; i < शब्दगण_2; i++) {
				RTLIL::Const previous = (*यह)[addr + i];
				क्रम(पूर्णांक j = 0; j < तथ्य_width_2; j++)
					अगर(init.en[j] != State::S1)
						तथ्य.set(तथ्य_width_2 * i + j, previous[j]);
			}
			जोड़ो_concatenated(init.addr.as_int(), तथ्य);
		}
	}
}

MemContents::iterator & MemContents::iterator::चालक++() {
	स्वतः it = _memory->मूल्यावली_2.upper_bound(_addr);
	अगर(it == _memory->मूल्यावली_2.end()) {
		_memory = nullptr;
		_addr = ~(addr_t) 0;
	} अन्यथा
		_addr = it->first;
	वापस *यह;
}

व्योम MemContents::check() {
	log_assert(_addr_width > 0 && _addr_width < (पूर्णांक)माप(addr_t) * 8);
	log_assert(तथ्य_width_2 > 0);
	log_assert(default_मूल्य.आकार() == तथ्य_width_2);
	अगर(मूल्यावली_2.empty()) वापस;
	स्वतः it = मूल्यावली_2.begin();
	क्रम(;;) {
		log_assert(!it->second.empty());
		log_assert(it->second.आकार() % तथ्य_width_2 == 0);
		स्वतः end1 = _range_end(it);
		log_assert(_range_begin(it) < (addr_t)(1<<_addr_width));
		log_assert(end1 <= (addr_t)(1<<_addr_width));
		अगर(++it == मूल्यावली_2.end())
			अवरोध;
		// check that ranges neither overlap nor touch
		log_assert(_range_begin(it) > end1);
	}
}

bool MemContents::_range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) स्थिर {
	// if addr < begin, the subtraction will overflow, and the comparison will always fail
	// (since we have an invariant that begin + size <= 2^(addr_t bits))
	वापस it != मूल्यावली_2.end() && addr - _range_begin(it) < range_आकार(it);
}


bool MemContents::_range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) स्थिर {
	// note that we assume begin_addr <= end_addr
	वापस it != मूल्यावली_2.end() && _range_begin(it) <= begin_addr && end_addr - _range_begin(it) <= range_आकार(it);
}

bool MemContents::_range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) स्थिर {
	अगर(it == मूल्यावली_2.end() || begin_addr >= end_addr)
		वापस false;
	स्वतः top1 = _range_end(it) - 1;
	स्वतः top2 = end_addr - 1;
	वापस !(top1 < begin_addr || top2 < _range_begin(it));
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_range_at(addr_t addr) स्थिर {
	// allow addr == 1<<_addr_width (which will just return end())
	log_assert(addr <= (addr_t)(1<<_addr_width));
	// get the first range with base > addr
	// (we use const_cast since map::iterators are only passed around internally and not exposed to the user
	// and using map::iterator in both the const and non-const case simplifies the code a little,
	// at the cost of having to be a little careful when implementing const methods)
	स्वतः it = const_cast<std::map<addr_t, RTLIL::Const> &>(मूल्यावली_2).upper_bound(addr);
	// if we get the very first range, all ranges are past the addr, so return the first one
	अगर(it == मूल्यावली_2.begin())
		वापस it;
	// otherwise, go back to the previous interval
	// this must be the last interval with base <= addr
	स्वतः it_पूर्व = std::अग्र(it, -1);
	अगर(_range_contains(it_पूर्व, addr))
		वापस it_पूर्व;
	अन्यथा
		वापस it;
}

RTLIL::Const MemContents::चालक[](addr_t addr) स्थिर {
	स्वतः it = _range_at(addr);
	अगर(_range_contains(it, addr))
		वापस it->second.extract(_range_offset(it, addr), तथ्य_width_2);
	अन्यथा
		वापस default_मूल्य;
}

MemContents::addr_t MemContents::गिनती_range(addr_t begin_addr, addr_t end_addr) स्थिर {
	addr_t गिनती = 0;
	क्रम(स्वतः it = _range_at(begin_addr); _range_overlaps(it, begin_addr, end_addr); it++) {
		स्वतः first = std::max(_range_begin(it), begin_addr);
		स्वतः last = std::min(_range_end(it), end_addr);
		गिनती += last - first;
	}
	वापस गिनती;
}

व्योम MemContents::clear_range(addr_t begin_addr, addr_t end_addr) {
	अगर(begin_addr >= end_addr) वापस;
	// identify which ranges are affected by this operation
	// the first iterator affected is the first one containing any addr >= begin_addr
	स्वतः begin_it = _range_at(begin_addr);
	// the first iterator *not* affected is the first one with base addr > end_addr - 1
	स्वतः end_it = मूल्यावली_2.upper_bound(end_addr - 1);
	अगर(begin_it == end_it)
		वापस; // nothing to do
	// the last iterator affected is one before the first one not affected
	स्वतः last_it = std::अग्र(end_it, -1);
	// the first and last range may need to be truncated, the rest can just be deleted
	// to handle the begin_it == last_it case correctly, do the end case first by inserting a new range past the end
	अगर(_range_contains(last_it, end_addr - 1)) {
		स्वतः new_begin = end_addr;
		स्वतः end = _range_end(last_it);
		// if there is data past the end address, preserve it by creating a new range
		अगर(new_begin != end)
			end_it = मूल्यावली_2.emplace_hint(last_it, new_begin, last_it->second.extract(_range_offset(last_it, new_begin), (_range_end(last_it) - new_begin) * तथ्य_width_2));
		// the original range will either be truncated in the next if() block or deleted in the erase, so we can leave it untruncated
	}
	अगर(_range_contains(begin_it, begin_addr)) {
		स्वतः new_end = begin_addr;
		// if there is data before the start address, truncate but don't delete
		अगर(new_end != begin_it->first) {
			begin_it->second.extu(_range_offset(begin_it, new_end));
			++begin_it;
		}
		// else: begin_it will be deleted
	}
	मूल्यावली_2.erase(begin_it, end_it);
}

std::map<MemContents::addr_t, RTLIL::Const>::iterator MemContents::_reserve_range(addr_t begin_addr, addr_t end_addr) {
	अगर(begin_addr >= end_addr)
		वापस मूल्यावली_2.end(); // need a dummy value to return, end() is cheap
	// find the first range containing any addr >= begin_addr - 1
	स्वतः lower_it = begin_addr == 0 ? मूल्यावली_2.begin() : _range_at(begin_addr - 1);
	// check if our range is already covered by a single range
	// note that since ranges are not allowed to touch, if any range contains begin_addr, lower_it equals that range
	अगर (_range_contains(lower_it, begin_addr, end_addr))
		वापस lower_it;
	// find the first range containing any addr >= end_addr
	स्वतः upper_it = _range_at(end_addr);
	// check if either of the two ranges we just found touch our range
	bool lower_touch = begin_addr > 0 && _range_contains(lower_it, begin_addr - 1);
	bool upper_touch = _range_contains(upper_it, end_addr);
	अगर (lower_touch && upper_touch) {
		log_assert (lower_it != upper_it); // lower_it == upper_it should be excluded by the check above
		// we have two different ranges touching at either end, we need to merge them
		स्वतः upper_end = _range_end(upper_it);
		// make range bigger (maybe reserve here instead of resize?)
		lower_it->second.resize(_range_offset(lower_it, upper_end), State::Sx);
		// copy only the data beyond our range
		std::copy(range_तथ्य(upper_it, end_addr), range_तथ्य(upper_it, upper_end), range_तथ्य(lower_it, end_addr));
		// keep lower_it, but delete upper_it
		मूल्यावली_2.erase(std::अग्र(lower_it), std::अग्र(upper_it));
		वापस lower_it;
	} अन्यथा अगर (lower_touch) {
		// we have a range to the left, just make it bigger and delete any other that may exist.
		lower_it->second.resize(_range_offset(lower_it, end_addr), State::Sx);
		// keep lower_it and upper_it
		मूल्यावली_2.erase(std::अग्र(lower_it), upper_it);
		वापस lower_it;
	} अन्यथा अगर (upper_touch) {
		// we have a range to the right, we need to expand it
		// since we need to erase and reinsert to a new address, steal the data
		// note that begin_addr is not in upper_it, otherwise the whole range covered check would have tripped
		RTLIL::Const तथ्य(State::Sx, (_range_begin(upper_it) - begin_addr) * तथ्य_width_2);
		तथ्य.append(std::move(upper_it->second));
		// delete lower_it and upper_it, then reinsert
		मूल्यावली_2.erase(lower_it, std::अग्र(upper_it));
		वापस मूल्यावली_2.emplace(begin_addr, std::move(तथ्य)).first;
	} अन्यथा {
		// no ranges are touching, so just delete all ranges in our range and allocate a new one
		// could try to resize an existing range but not sure if that actually helps
		मूल्यावली_2.erase(lower_it, upper_it);
		वापस मूल्यावली_2.emplace(begin_addr, RTLIL::Const(State::Sx, (end_addr - begin_addr) * तथ्य_width_2)).first;
	}
}

व्योम MemContents::जोड़ो_concatenated(addr_t addr, RTLIL::Const स्थिर &मूल्यावली) {
	addr_t शब्दगण_2 = (मूल्यावली.आकार() + तथ्य_width_2 - 1) / तथ्य_width_2;
	log_assert(addr < (addr_t)(1<<_addr_width));
	log_assert(शब्दगण_2 <= (addr_t)(1<<_addr_width) - addr);
	स्वतः it = _reserve_range(addr, addr + शब्दगण_2);
	स्वतः to_begin = range_तथ्य(it, addr);
	std::copy(मूल्यावली.begin(), मूल्यावली.end(), to_begin);
	// if values is not word-aligned, fill any missing bits with 0
	std::fill(to_begin + मूल्यावली.आकार(), to_begin + शब्दगण_2 * तथ्य_width_2, State::S0);
}

RTLIL::Const::iterator MemContents::_range_write(RTLIL::Const::iterator it, RTLIL::Const स्थिर &word) {
	स्वतः from_end = word.आकार() <= तथ्य_width_2 ? word.end() : word.begin() + तथ्य_width_2;
	स्वतः to_end = std::copy(word.begin(), from_end, it);
	स्वतः it_अग्र = std::अग्र(it, तथ्य_width_2);
	std::fill(to_end, it_अग्र, State::S0);
	वापस it_अग्र;
}
