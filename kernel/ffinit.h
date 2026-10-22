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

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساختار FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	تهی set(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		برای (auto wire : module->wires())
			اگر (wire->attributes.شمار(ID::init))
				process_wire(wire);
	}

	تهی process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		برای (صحیح i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			اگر (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				ادامه;

			اگر (initbits.شمار(bit)) {
				اگر (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				ادامه;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	تهی set_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](ثابت ParallelDispatchThreadPool::RunCtx &ctx) {
			برای (صحیح i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				اگر (wire->attributes.شمار(ID::init))
					init_wires.درج(ctx, wire);
			}
		});

		برای (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) ثابت
	{
		auto it = initbits.find((*sigmap)(bit));
		اگر (it != initbits.end())
			بازگشت it->second.first;
		وگرنه
			بازگشت State::Sx;
	}

	RTLIL::Const operator()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		برای (auto bit : sig)
			res_bits.افزودن_back((*this)(bit));
		بازگشت res_bits.build();
	}

	تهی set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		اگر (it != initbits.end())
			abit = it->second.second;
		وگرنه اگر (val == State::Sx)
			بازگشت;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		اگر (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			اگر (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} وگرنه اگر (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	تهی set_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		برای (صحیح i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	تهی remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	تهی remove_init(ثابت RTLIL::SigSpec &sig)
	{
		برای (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	تهی move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		برای (auto cell : module->cells()) {
			اگر (cell->type != ID($barrier))
				ادامه;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			برای (صحیح i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		اگر (next.empty() || initbits.empty())
			بازگشت;
		pool<RTLIL::SigBit> inputs = درون_داد_nets(module), moved;
		برای (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			اگر (val == State::Sx)
				ادامه;
			moved.درج(from);
			برای (صحیح i = 0; i < GetSize(next) && next.شمار((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			اگر (to_net.wire == nullptr || inputs.شمار(to_net))
				ادامه;
			RTLIL::State to_val = (*this)(to);
			اگر (to_val == State::Sx)
				set_init(to, val);
			وگرنه اگر (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		اگر (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> درون_داد_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::SigBit> nets;
		برای (auto wire : module->wires())
			اگر (wire->port_درون_داد)
				برای (auto bit : (*sigmap)(wire))
					nets.درج(bit);
		بازگشت nets;
	}

	تهی remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::SigBit> &nets)
	{
		برای (auto bit : nets)
			initbits.erase(bit);
		برای (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			اگر (it == wire->attributes.end())
				ادامه;
			برای (صحیح i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				اگر (nets.شمار((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			اگر (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	تهی clear()
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
