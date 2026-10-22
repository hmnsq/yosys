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

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

sa_nracanaa FfInitVals
{
	const SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	void set(const SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		ꦏꦁꦒꦺꦴ (auto wire : module->wires())
			ꦪꦼꦤ꧀ (wire->attributes.count(ID::init))
				process_wire(wire);
	}

	void process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			ꦪꦼꦤ꧀ (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				continue;

			ꦪꦼꦤ꧀ (initbits.count(bit)) {
				ꦪꦼꦤ꧀ (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				continue;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	void set_parallel(const SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		const RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](const ParallelDispatchThreadPool::RunCtx &ctx) {
			ꦏꦁꦒꦺꦴ (puur_naa_nka i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ꦪꦼꦤ꧀ (wire->attributes.count(ID::init))
					init_wires.insert(ctx, wire);
			}
		});

		ꦏꦁꦒꦺꦴ (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) const
	{
		auto it = initbits.find((*sigmap)(bit));
		ꦪꦼꦤ꧀ (it != initbits.end())
			ꦮꦭꦶ it->second.first;
		ꦭꦶꦪ
			ꦮꦭꦶ State::Sx;
	}

	RTLIL::Const operator()(const RTLIL::SigSpec &sig) const
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		ꦏꦁꦒꦺꦴ (auto bit : sig)
			res_bits.push_back((*this)(bit));
		ꦮꦭꦶ res_bits.build();
	}

	void set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		ꦪꦼꦤ꧀ (it != initbits.end())
			abit = it->second.second;
		ꦭꦶꦪ ꦪꦼꦤ꧀ (val == State::Sx)
			ꦮꦭꦶ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		ꦪꦼꦤ꧀ (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			ꦪꦼꦤ꧀ (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} ꦭꦶꦪ ꦪꦼꦤ꧀ (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	void set_init(const RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	void remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	void remove_init(const RTLIL::SigSpec &sig)
	{
		ꦏꦁꦒꦺꦴ (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	void move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		ꦏꦁꦒꦺꦴ (auto cell : module->cells()) {
			ꦪꦼꦤ꧀ (cell->type != ID($barrier))
				continue;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		ꦪꦼꦤ꧀ (next.empty() || initbits.empty())
			ꦮꦭꦶ;
		pool<RTLIL::SigBit> inputs = input_nets(module), moved;
		ꦏꦁꦒꦺꦴ (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			ꦪꦼꦤ꧀ (val == State::Sx)
				continue;
			moved.insert(from);
			ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(next) && next.count((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			ꦪꦼꦤ꧀ (to_net.wire == nullptr || inputs.count(to_net))
				continue;
			RTLIL::State to_val = (*this)(to);
			ꦪꦼꦤ꧀ (to_val == State::Sx)
				set_init(to, val);
			ꦭꦶꦪ ꦪꦼꦤ꧀ (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		ꦪꦼꦤ꧀ (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> input_nets(RTLIL::Module *module) const
	{
		pool<RTLIL::SigBit> nets;
		ꦏꦁꦒꦺꦴ (auto wire : module->wires())
			ꦪꦼꦤ꧀ (wire->port_input)
				ꦏꦁꦒꦺꦴ (auto bit : (*sigmap)(wire))
					nets.insert(bit);
		ꦮꦭꦶ nets;
	}

	void remove_net_inits(RTLIL::Module *module, const pool<RTLIL::SigBit> &nets)
	{
		ꦏꦁꦒꦺꦴ (auto bit : nets)
			initbits.erase(bit);
		ꦏꦁꦒꦺꦴ (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			ꦪꦼꦤ꧀ (it == wire->attributes.end())
				continue;
			ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				ꦪꦼꦤ꧀ (nets.count((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			ꦪꦼꦤ꧀ (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	void clear()
	{
		initbits.clear();
	}

	FfInitVals (const SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
