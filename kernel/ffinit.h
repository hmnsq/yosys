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

#ifndef FFINIT_H
#define FFINIT_H

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ഘടന FfInitVals
{
	സ്ഥിരം SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	ശൂന്യം set(സ്ഥിരം SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		വേണ്ടി (auto wire : module->wires())
			എങ്കിൽ (wire->attributes.എണ്ണം(ID::init))
				process_wire(wire);
	}

	ശൂന്യം process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			എങ്കിൽ (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				തുടരുക;

			എങ്കിൽ (initbits.എണ്ണം(bit)) {
				എങ്കിൽ (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				തുടരുക;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	ശൂന്യം set_parallel(സ്ഥിരം SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		സ്ഥിരം RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](സ്ഥിരം ParallelDispatchThreadPool::RunCtx &ctx) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				എങ്കിൽ (wire->attributes.എണ്ണം(ID::init))
					init_wires.തിരുകുക(ctx, wire);
			}
		});

		വേണ്ടി (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) സ്ഥിരം
	{
		auto it = initbits.find((*sigmap)(bit));
		എങ്കിൽ (it != initbits.end())
			തിരികെ it->second.first;
		അല്ലെങ്കിൽ
			തിരികെ State::Sx;
	}

	RTLIL::Const operator()(സ്ഥിരം RTLIL::SigSpec &sig) സ്ഥിരം
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		വേണ്ടി (auto bit : sig)
			res_bits.തള്ളുക_back((*this)(bit));
		തിരികെ res_bits.build();
	}

	ശൂന്യം set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		എങ്കിൽ (it != initbits.end())
			abit = it->second.second;
		അല്ലെങ്കിൽ എങ്കിൽ (val == State::Sx)
			തിരികെ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		എങ്കിൽ (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			എങ്കിൽ (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} അല്ലെങ്കിൽ എങ്കിൽ (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	ശൂന്യം set_init(സ്ഥിരം RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	ശൂന്യം remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	ശൂന്യം remove_init(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	ശൂന്യം move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		വേണ്ടി (auto cell : module->cells()) {
			എങ്കിൽ (cell->type != ID($barrier))
				തുടരുക;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		എങ്കിൽ (next.empty() || initbits.empty())
			തിരികെ;
		pool<RTLIL::SigBit> inputs = നിവേശം_nets(module), moved;
		വേണ്ടി (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			എങ്കിൽ (val == State::Sx)
				തുടരുക;
			moved.തിരുകുക(from);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(next) && next.എണ്ണം((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			എങ്കിൽ (to_net.wire == nullptr || inputs.എണ്ണം(to_net))
				തുടരുക;
			RTLIL::State to_val = (*this)(to);
			എങ്കിൽ (to_val == State::Sx)
				set_init(to, val);
			അല്ലെങ്കിൽ എങ്കിൽ (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		എങ്കിൽ (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> നിവേശം_nets(RTLIL::Module *module) സ്ഥിരം
	{
		pool<RTLIL::SigBit> nets;
		വേണ്ടി (auto wire : module->wires())
			എങ്കിൽ (wire->port_നിവേശം)
				വേണ്ടി (auto bit : (*sigmap)(wire))
					nets.തിരുകുക(bit);
		തിരികെ nets;
	}

	ശൂന്യം remove_net_inits(RTLIL::Module *module, സ്ഥിരം pool<RTLIL::SigBit> &nets)
	{
		വേണ്ടി (auto bit : nets)
			initbits.erase(bit);
		വേണ്ടി (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			എങ്കിൽ (it == wire->attributes.end())
				തുടരുക;
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				എങ്കിൽ (nets.എണ്ണം((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			എങ്കിൽ (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	ശൂന്യം clear()
	{
		initbits.clear();
	}

	FfInitVals (സ്ഥിരം SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
