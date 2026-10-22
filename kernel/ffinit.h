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

#अगर_अघोषित FFINIT_H
#घोषणा FFINIT_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा FfInitVals
{
	स्थिर SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	व्योम set(स्थिर SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		क्रम (स्वतः wire : module->wires())
			अगर (wire->attributes.गिनती(ID::init))
				process_wire(wire);
	}

	व्योम process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		क्रम (पूर्णांक i = 0; i < Get_आकार(wirebits) && i < Get_आकार(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			अगर (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				जारी;

			अगर (initbits.गिनती(bit)) {
				अगर (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				जारी;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	व्योम set_parallel(स्थिर SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		स्थिर RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_आकार(0, module->wires_आकार(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](स्थिर ParallelDispatchThreadPool::RunCtx &ctx) {
			क्रम (पूर्णांक i : ctx.item_range(const_module->wires_आकार())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				अगर (wire->attributes.गिनती(ID::init))
					init_wires.जोड़ो(ctx, wire);
			}
		});

		क्रम (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State चालक()(RTLIL::SigBit bit) स्थिर
	{
		स्वतः it = initbits.find((*sigmap)(bit));
		अगर (it != initbits.end())
			वापस it->second.first;
		अन्यथा
			वापस State::Sx;
	}

	RTLIL::Const चालक()(स्थिर RTLIL::SigSpec &sig) स्थिर
	{
		RTLIL::Const::Builder res_bits(Get_आकार(sig));
		क्रम (स्वतः bit : sig)
			res_bits.धकेलो_back((*this)(bit));
		वापस res_bits.build();
	}

	व्योम set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		स्वतः it = initbits.find(mbit);
		अगर (it != initbits.end())
			abit = it->second.second;
		अन्यथा अगर (val == State::Sx)
			वापस;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		स्वतः it2 = abit.wire->attributes.find(ID::init);
		अगर (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			अगर (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} अन्यथा अगर (val != State::Sx) {
			Const cval(State::Sx, Get_आकार(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	व्योम set_init(स्थिर RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_आकार(sig) == Get_आकार(val));
		क्रम (पूर्णांक i = 0; i < Get_आकार(sig); i++)
			set_init(sig[i], val[i]);
	}

	व्योम remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	व्योम remove_init(स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (स्वतः bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	व्योम move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> अग्र;
		क्रम (स्वतः cell : module->cells()) {
			अगर (cell->type != ID($barrier))
				जारी;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			क्रम (पूर्णांक i = 0; i < Get_आकार(sig_y); i++)
				अग्र[sig_y[i]] = sig_a[i];
		}
		अगर (अग्र.empty() || initbits.empty())
			वापस;
		pool<RTLIL::SigBit> inputs = निवेश_nets(module), moved;
		क्रम (स्वतः [from, to] : अग्र) {
			RTLIL::State val = (*this)(from);
			अगर (val == State::Sx)
				जारी;
			moved.जोड़ो(from);
			क्रम (पूर्णांक i = 0; i < Get_आकार(अग्र) && अग्र.गिनती((*sigmap)(to)); i++)
				to = अग्र.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			अगर (to_net.wire == nullptr || inputs.गिनती(to_net))
				जारी;
			RTLIL::State to_val = (*this)(to);
			अगर (to_val == State::Sx)
				set_init(to, val);
			अन्यथा अगर (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		अगर (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> निवेश_nets(RTLIL::Module *module) स्थिर
	{
		pool<RTLIL::SigBit> nets;
		क्रम (स्वतः wire : module->wires())
			अगर (wire->port_निवेश)
				क्रम (स्वतः bit : (*sigmap)(wire))
					nets.जोड़ो(bit);
		वापस nets;
	}

	व्योम remove_net_inits(RTLIL::Module *module, स्थिर pool<RTLIL::SigBit> &nets)
	{
		क्रम (स्वतः bit : nets)
			initbits.erase(bit);
		क्रम (स्वतः wire : module->wires()) {
			स्वतः it = wire->attributes.find(ID::init);
			अगर (it == wire->attributes.end())
				जारी;
			क्रम (पूर्णांक i = 0; i < Get_आकार(wire) && i < Get_आकार(it->second); i++)
				अगर (nets.गिनती((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			अगर (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	व्योम clear()
	{
		initbits.clear();
	}

	FfInitVals (स्थिर SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#पूर्ण_अगर
