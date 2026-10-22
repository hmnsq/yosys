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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு FfInitVals
{
	மாறிலி SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	வெறுமை set(மாறிலி SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		க்கு (auto wire : module->wires())
			எனில் (wire->attributes.மொத்தம்(ID::init))
				process_wire(wire);
	}

	வெறுமை process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		க்கு (முழு i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			எனில் (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				தொடர்;

			எனில் (initbits.மொத்தம்(bit)) {
				எனில் (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				தொடர்;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	வெறுமை set_parallel(மாறிலி SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		மாறிலி RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](மாறிலி ParallelDispatchThreadPool::RunCtx &ctx) {
			க்கு (முழு i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				எனில் (wire->attributes.மொத்தம்(ID::init))
					init_wires.செருகு(ctx, wire);
			}
		});

		க்கு (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) மாறிலி
	{
		auto it = initbits.find((*sigmap)(bit));
		எனில் (it != initbits.end())
			திரும்பு it->second.first;
		இல்லையெனில்
			திரும்பு State::Sx;
	}

	RTLIL::Const operator()(மாறிலி RTLIL::SigSpec &sig) மாறிலி
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		க்கு (auto bit : sig)
			res_bits.தள்ளு_back((*this)(bit));
		திரும்பு res_bits.build();
	}

	வெறுமை set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		எனில் (it != initbits.end())
			abit = it->second.second;
		இல்லையெனில் எனில் (val == State::Sx)
			திரும்பு;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		எனில் (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			எனில் (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} இல்லையெனில் எனில் (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	வெறுமை set_init(மாறிலி RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		க்கு (முழு i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	வெறுமை remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	வெறுமை remove_init(மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	வெறுமை move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		க்கு (auto cell : module->cells()) {
			எனில் (cell->type != ID($barrier))
				தொடர்;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			க்கு (முழு i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		எனில் (next.empty() || initbits.empty())
			திரும்பு;
		pool<RTLIL::SigBit> inputs = உள்ளீடு_nets(module), moved;
		க்கு (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			எனில் (val == State::Sx)
				தொடர்;
			moved.செருகு(from);
			க்கு (முழு i = 0; i < GetSize(next) && next.மொத்தம்((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			எனில் (to_net.wire == nullptr || inputs.மொத்தம்(to_net))
				தொடர்;
			RTLIL::State to_val = (*this)(to);
			எனில் (to_val == State::Sx)
				set_init(to, val);
			இல்லையெனில் எனில் (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		எனில் (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> உள்ளீடு_nets(RTLIL::Module *module) மாறிலி
	{
		pool<RTLIL::SigBit> nets;
		க்கு (auto wire : module->wires())
			எனில் (wire->port_உள்ளீடு)
				க்கு (auto bit : (*sigmap)(wire))
					nets.செருகு(bit);
		திரும்பு nets;
	}

	வெறுமை remove_net_inits(RTLIL::Module *module, மாறிலி pool<RTLIL::SigBit> &nets)
	{
		க்கு (auto bit : nets)
			initbits.erase(bit);
		க்கு (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			எனில் (it == wire->attributes.end())
				தொடர்;
			க்கு (முழு i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				எனில் (nets.மொத்தம்((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			எனில் (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	வெறுமை clear()
	{
		initbits.clear();
	}

	FfInitVals (மாறிலி SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
