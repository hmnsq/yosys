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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା FfInitVals
{
	ଅଚଳ SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	ଶୂନ୍ୟ set(ଅଚଳ SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		ପାଇଁ (auto wire : module->wires())
			ଯଦି (wire->attributes.ଗଣନା_2(ID::init))
				process_wire(wire);
	}

	ଶୂନ୍ୟ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			ଯଦି (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				ଜାରି;

			ଯଦି (initbits.ଗଣନା_2(bit)) {
				ଯଦି (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				ଜାରି;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	ଶୂନ୍ୟ set_parallel(ଅଚଳ SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ଅଚଳ RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](ଅଚଳ ParallelDispatchThreadPool::RunCtx &ctx) {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ଯଦି (wire->attributes.ଗଣନା_2(ID::init))
					init_wires.ଭର୍ତ୍ତି(ctx, wire);
			}
		});

		ପାଇଁ (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) ଅଚଳ
	{
		auto it = initbits.find((*sigmap)(bit));
		ଯଦି (it != initbits.end())
			ଫେରନ୍ତୁ it->second.first;
		ଅନ୍ୟଥା
			ଫେରନ୍ତୁ State::Sx;
	}

	RTLIL::Const operator()(ଅଚଳ RTLIL::SigSpec &sig) ଅଚଳ
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		ପାଇଁ (auto bit : sig)
			res_bits.ଠେଲ_back((*this)(bit));
		ଫେରନ୍ତୁ res_bits.build();
	}

	ଶୂନ୍ୟ set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		ଯଦି (it != initbits.end())
			abit = it->second.second;
		ଅନ୍ୟଥା ଯଦି (val == State::Sx)
			ଫେରନ୍ତୁ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		ଯଦି (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			ଯଦି (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} ଅନ୍ୟଥା ଯଦି (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	ଶୂନ୍ୟ set_init(ଅଚଳ RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	ଶୂନ୍ୟ remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	ଶୂନ୍ୟ remove_init(ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	ଶୂନ୍ୟ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		ପାଇଁ (auto cell : module->cells()) {
			ଯଦି (cell->type != ID($barrier))
				ଜାରି;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		ଯଦି (next.empty() || initbits.empty())
			ଫେରନ୍ତୁ;
		pool<RTLIL::SigBit> inputs = ନିବେଶ_nets(module), moved;
		ପାଇଁ (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			ଯଦି (val == State::Sx)
				ଜାରି;
			moved.ଭର୍ତ୍ତି(from);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(next) && next.ଗଣନା_2((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			ଯଦି (to_net.wire == nullptr || inputs.ଗଣନା_2(to_net))
				ଜାରି;
			RTLIL::State to_val = (*this)(to);
			ଯଦି (to_val == State::Sx)
				set_init(to, val);
			ଅନ୍ୟଥା ଯଦି (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		ଯଦି (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> ନିବେଶ_nets(RTLIL::Module *module) ଅଚଳ
	{
		pool<RTLIL::SigBit> nets;
		ପାଇଁ (auto wire : module->wires())
			ଯଦି (wire->port_ନିବେଶ)
				ପାଇଁ (auto bit : (*sigmap)(wire))
					nets.ଭର୍ତ୍ତି(bit);
		ଫେରନ୍ତୁ nets;
	}

	ଶୂନ୍ୟ remove_net_inits(RTLIL::Module *module, ଅଚଳ pool<RTLIL::SigBit> &nets)
	{
		ପାଇଁ (auto bit : nets)
			initbits.erase(bit);
		ପାଇଁ (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			ଯଦି (it == wire->attributes.end())
				ଜାରି;
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				ଯଦି (nets.ଗଣନା_2((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			ଯଦି (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	ଶୂନ୍ୟ clear()
	{
		initbits.clear();
	}

	FfInitVals (ଅଚଳ SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
