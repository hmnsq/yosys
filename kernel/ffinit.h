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

struct FfInitVals
{
	const SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	void set(const SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		위해 (auto wire : module->wires())
			만약 (wire->attributes.count(ID::init))
				process_wire(wire);
	}

	void process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		위해 (정수 i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			만약 (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				continue;

			만약 (initbits.count(bit)) {
				만약 (initbits.at(bit).first != val)
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
			위해 (정수 i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				만약 (wire->attributes.count(ID::init))
					init_wires.insert(ctx, wire);
			}
		});

		위해 (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) const
	{
		auto it = initbits.find((*sigmap)(bit));
		만약 (it != initbits.end())
			반환 it->second.first;
		아니면
			반환 State::Sx;
	}

	RTLIL::Const operator()(const RTLIL::SigSpec &sig) const
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		위해 (auto bit : sig)
			res_bits.push_back((*this)(bit));
		반환 res_bits.build();
	}

	void set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		만약 (it != initbits.end())
			abit = it->second.second;
		아니면 만약 (val == State::Sx)
			반환;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		만약 (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			만약 (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} 아니면 만약 (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	void set_init(const RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		위해 (정수 i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	void remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	void remove_init(const RTLIL::SigSpec &sig)
	{
		위해 (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	void move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		위해 (auto cell : module->cells()) {
			만약 (cell->type != ID($barrier))
				continue;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			위해 (정수 i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		만약 (next.empty() || initbits.empty())
			반환;
		pool<RTLIL::SigBit> inputs = input_nets(module), moved;
		위해 (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			만약 (val == State::Sx)
				continue;
			moved.insert(from);
			위해 (정수 i = 0; i < GetSize(next) && next.count((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			만약 (to_net.wire == nullptr || inputs.count(to_net))
				continue;
			RTLIL::State to_val = (*this)(to);
			만약 (to_val == State::Sx)
				set_init(to, val);
			아니면 만약 (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		만약 (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> input_nets(RTLIL::Module *module) const
	{
		pool<RTLIL::SigBit> nets;
		위해 (auto wire : module->wires())
			만약 (wire->port_input)
				위해 (auto bit : (*sigmap)(wire))
					nets.insert(bit);
		반환 nets;
	}

	void remove_net_inits(RTLIL::Module *module, const pool<RTLIL::SigBit> &nets)
	{
		위해 (auto bit : nets)
			initbits.erase(bit);
		위해 (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			만약 (it == wire->attributes.end())
				continue;
			위해 (정수 i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				만약 (nets.count((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			만약 (it->second.is_fully_undef())
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
