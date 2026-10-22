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

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

بنية FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	فراغ set(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		لكل (auto wire : module->wires())
			𐤀𐤌 (wire->attributes.count(ID::init))
				process_wire(wire);
	}

	فراغ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		لكل (صحيح i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			𐤀𐤌 (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				استمر;

			𐤀𐤌 (initbits.count(bit)) {
				𐤀𐤌 (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				استمر;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	فراغ set_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](ثابت ParallelDispatchThreadPool::RunCtx &ctx) {
			لكل (صحيح i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				𐤀𐤌 (wire->attributes.count(ID::init))
					init_wires.insert(ctx, wire);
			}
		});

		لكل (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) ثابت
	{
		auto it = initbits.find((*sigmap)(bit));
		𐤀𐤌 (it != initbits.end())
			أرجع it->second.first;
		وإلا
			أرجع State::Sx;
	}

	RTLIL::Const operator()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		لكل (auto bit : sig)
			res_bits.push_back((*this)(bit));
		أرجع res_bits.build();
	}

	فراغ set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		𐤀𐤌 (it != initbits.end())
			abit = it->second.second;
		وإلا 𐤀𐤌 (val == State::Sx)
			أرجع;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		𐤀𐤌 (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			𐤀𐤌 (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} وإلا 𐤀𐤌 (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	فراغ set_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		لكل (صحيح i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	فراغ remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	فراغ remove_init(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	فراغ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		لكل (auto cell : module->cells()) {
			𐤀𐤌 (cell->type != ID($barrier))
				استمر;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			لكل (صحيح i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		𐤀𐤌 (next.empty() || initbits.empty())
			أرجع;
		pool<RTLIL::SigBit> inputs = input_nets(module), moved;
		لكل (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			𐤀𐤌 (val == State::Sx)
				استمر;
			moved.insert(from);
			لكل (صحيح i = 0; i < GetSize(next) && next.count((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			𐤀𐤌 (to_net.wire == nullptr || inputs.count(to_net))
				استمر;
			RTLIL::State to_val = (*this)(to);
			𐤀𐤌 (to_val == State::Sx)
				set_init(to, val);
			وإلا 𐤀𐤌 (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		𐤀𐤌 (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> input_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::SigBit> nets;
		لكل (auto wire : module->wires())
			𐤀𐤌 (wire->port_input)
				لكل (auto bit : (*sigmap)(wire))
					nets.insert(bit);
		أرجع nets;
	}

	فراغ remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::SigBit> &nets)
	{
		لكل (auto bit : nets)
			initbits.erase(bit);
		لكل (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			𐤀𐤌 (it == wire->attributes.end())
				استمر;
			لكل (صحيح i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				𐤀𐤌 (nets.count((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			𐤀𐤌 (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	فراغ clear()
	{
		initbits.clear();
	}

	FfInitVals (ثابت SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
