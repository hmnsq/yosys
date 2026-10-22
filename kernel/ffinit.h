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

#यदि_अपरिभाषितम् FFINIT_H
#परिभाषय FFINIT_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

संरचना FfInitVals
{
	ध्रुव SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	शून्य समुच्चयः(ध्रुव SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		प्रति (स्वचालित wire : module->wires())
			यदि (wire->गुणाः.गणनम्(ID::init))
				process_wire(wire);
	}

	शून्य process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->गुणाः.at(ID::init);

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wirebits) && i < Get_आकार(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			यदि (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				अनुवर्त;

			यदि (initbits.गणनम्(bit)) {
				यदि (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				अनुवर्त;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	शून्य समुच्चयः_parallel(ध्रुव SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ध्रुव RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_आकार(0, module->wires_आकार(), 1000));
		Sharded_सदिशम्<RTLIL::Wire*> init_wires(subpool);
		subpool.सततम्([const_module, &init_wires](ध्रुव ParallelDispatchThreadPool::सततम्_Ctx &ctx) {
			प्रति (पूर्णाङ्क i : ctx.item_range(const_module->wires_आकार())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				यदि (wire->गुणाः.गणनम्(ID::init))
					init_wires.निवेशय(ctx, wire);
			}
		});

		प्रति (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State संक्रियकः()(RTLIL::SigBit bit) ध्रुव
	{
		स्वचालित it = initbits.find((*sigmap)(bit));
		यदि (it != initbits.अन्तः())
			प्रत्यागम it->second.first;
		अन्यथा
			प्रत्यागम State::Sx;
	}

	RTLIL::Const संक्रियकः()(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		RTLIL::Const::Builder res_द्वयङ्काः(Get_आकार(sig));
		प्रति (स्वचालित bit : sig)
			res_द्वयङ्काः.प्रक्षिप_पृष्ठभागः((*this)(bit));
		प्रत्यागम res_द्वयङ्काः.build();
	}

	शून्य समुच्चयः_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		स्वचालित it = initbits.find(mbit);
		यदि (it != initbits.अन्तः())
			abit = it->second.second;
		अन्यथा यदि (val == State::Sx)
			प्रत्यागम;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		स्वचालित it2 = abit.wire->गुणाः.find(ID::init);
		यदि (it2 != abit.wire->गुणाः.अन्तः()) {
			it2->second.समुच्चयः(abit.विस्थापनम्, val);
			यदि (it2->second.is_fully_undef())
				abit.wire->गुणाः.erase(it2);
		} अन्यथा यदि (val != State::Sx) {
			Const cval(State::Sx, Get_आकार(abit.wire));
			cval.समुच्चयः(abit.विस्थापनम्, val);
			abit.wire->गुणाः[ID::init] = cval;
		}
	}

	शून्य समुच्चयः_init(ध्रुव RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_आकार(sig) == Get_आकार(val));
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sig); i++)
			समुच्चयः_init(sig[i], val[i]);
	}

	शून्य remove_init(RTLIL::SigBit bit)
	{
		समुच्चयः_init(bit, State::Sx);
	}

	शून्य remove_init(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (स्वचालित bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	शून्य move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> अग्र;
		प्रति (स्वचालित cell : module->cells()) {
			यदि (cell->type != ID($barrier))
				अनुवर्त;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_पत्तनम्(ID::Y)), sig_a = cell->get_पत्तनम्(ID::A);
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sig_y); i++)
				अग्र[sig_y[i]] = sig_a[i];
		}
		यदि (अग्र.रिक्तानि() || initbits.रिक्तानि())
			प्रत्यागम;
		pool<RTLIL::SigBit> inputs = निवेशः_nets(module), moved;
		प्रति (स्वचालित [from, गन्तव्यम्] : अग्र) {
			RTLIL::State val = (*this)(from);
			यदि (val == State::Sx)
				अनुवर्त;
			moved.निवेशय(from);
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(अग्र) && अग्र.गणनम्((*sigmap)(गन्तव्यम्)); i++)
				गन्तव्यम् = अग्र.at((*sigmap)(गन्तव्यम्));
			RTLIL::SigBit गन्तव्यम्_net = (*sigmap)(गन्तव्यम्);
			यदि (गन्तव्यम्_net.wire == nullptr || inputs.गणनम्(गन्तव्यम्_net))
				अनुवर्त;
			RTLIL::State गन्तव्यम्_val = (*this)(गन्तव्यम्);
			यदि (गन्तव्यम्_val == State::Sx)
				समुच्चयः_init(गन्तव्यम्, val);
			अन्यथा यदि (गन्तव्यम्_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(गन्तव्यम्), log_signal(from), log_signal(val), log_signal(गन्तव्यम्_val));
		}
		यदि (!moved.रिक्तानि())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> निवेशः_nets(RTLIL::Module *module) ध्रुव
	{
		pool<RTLIL::SigBit> nets;
		प्रति (स्वचालित wire : module->wires())
			यदि (wire->पत्तनम्_निवेशः)
				प्रति (स्वचालित bit : (*sigmap)(wire))
					nets.निवेशय(bit);
		प्रत्यागम nets;
	}

	शून्य remove_net_inits(RTLIL::Module *module, ध्रुव pool<RTLIL::SigBit> &nets)
	{
		प्रति (स्वचालित bit : nets)
			initbits.erase(bit);
		प्रति (स्वचालित wire : module->wires()) {
			स्वचालित it = wire->गुणाः.find(ID::init);
			यदि (it == wire->गुणाः.अन्तः())
				अनुवर्त;
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wire) && i < Get_आकार(it->second); i++)
				यदि (nets.गणनम्((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.समुच्चयः(i, State::Sx);
			यदि (it->second.is_fully_undef())
				wire->गुणाः.erase(it);
		}
	}

	शून्य clear()
	{
		initbits.clear();
	}

	FfInitVals (ध्रुव SigMapView *sigmap, RTLIL::Module *module)
	{
		समुच्चयः(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
