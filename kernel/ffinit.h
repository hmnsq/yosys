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
		ສຳລັບ (auto wire : module->wires())
			ຖ້າ (wire->attributes.count(ID::init))
				process_wire(wire);
	}

	void process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			ຖ້າ (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				continue;

			ຖ້າ (initbits.count(bit)) {
				ຖ້າ (initbits.at(bit).first != val)
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
			ສຳລັບ (ຈຳນວນເຕັມ i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ຖ້າ (wire->attributes.count(ID::init))
					init_wires.insert(ctx, wire);
			}
		});

		ສຳລັບ (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) const
	{
		auto it = initbits.find((*sigmap)(bit));
		ຖ້າ (it != initbits.end())
			ສົ່ງຄືນ it->second.first;
		ບໍ່ດັ່ງນັ້ນ
			ສົ່ງຄືນ State::Sx;
	}

	RTLIL::Const operator()(const RTLIL::SigSpec &sig) const
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		ສຳລັບ (auto bit : sig)
			res_bits.push_back((*this)(bit));
		ສົ່ງຄືນ res_bits.build();
	}

	void set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		ຖ້າ (it != initbits.end())
			abit = it->second.second;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (val == State::Sx)
			ສົ່ງຄືນ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		ຖ້າ (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			ຖ້າ (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	void set_init(const RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	void remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	void remove_init(const RTLIL::SigSpec &sig)
	{
		ສຳລັບ (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	void move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		ສຳລັບ (auto cell : module->cells()) {
			ຖ້າ (cell->type != ID($barrier))
				continue;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		ຖ້າ (next.empty() || initbits.empty())
			ສົ່ງຄືນ;
		pool<RTLIL::SigBit> inputs = input_nets(module), moved;
		ສຳລັບ (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			ຖ້າ (val == State::Sx)
				continue;
			moved.insert(from);
			ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(next) && next.count((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			ຖ້າ (to_net.wire == nullptr || inputs.count(to_net))
				continue;
			RTLIL::State to_val = (*this)(to);
			ຖ້າ (to_val == State::Sx)
				set_init(to, val);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		ຖ້າ (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> input_nets(RTLIL::Module *module) const
	{
		pool<RTLIL::SigBit> nets;
		ສຳລັບ (auto wire : module->wires())
			ຖ້າ (wire->port_input)
				ສຳລັບ (auto bit : (*sigmap)(wire))
					nets.insert(bit);
		ສົ່ງຄືນ nets;
	}

	void remove_net_inits(RTLIL::Module *module, const pool<RTLIL::SigBit> &nets)
	{
		ສຳລັບ (auto bit : nets)
			initbits.erase(bit);
		ສຳລັບ (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			ຖ້າ (it == wire->attributes.end())
				continue;
			ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				ຖ້າ (nets.count((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			ຖ້າ (it->second.is_fully_undef())
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
