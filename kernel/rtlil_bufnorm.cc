/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/modtools.h"

#شامل <string.h>
#شامل <algorithm>
#شامل <optional>

YOSYS_NAMESPACE_BEGIN


ایستا تهی buf_norm_seed_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	برای (auto cell : module->cells())
	برای (auto &conn : cell->connections()) {
		اگر (GetSize(conn.second) == 0 || (cell->port_dir(conn.first) != RTLIL::PD_OUTPUT && cell->port_dir(conn.first) != RTLIL::PD_INOUT))
			ادامه;
		module->buf_norm_cell_queue.درج(cell);
		module->buf_norm_cell_port_queue.emplace(cell, conn.first);
	}
	برای (auto wire : module->wires())
		module->buf_norm_wire_queue.درج(wire);
}

تهی RTLIL::Design::bufNormalize(منطقی enable)
{
	اگر (!enable)
	{
		اگر (!flagBufferedNormalized)
			بازگشت;

		برای (auto module : modules()) {
			module->buf_norm_cell_queue.clear();
			module->buf_norm_wire_queue.clear();
			module->buf_norm_cell_port_queue.clear();
			برای (auto wire : module->wires()) {
				wire->driverCell_ = nullptr;
				wire->driverPort_ = IdString();
			}
			module->buf_norm_connect_index.clear();
		}

		flagBufferedNormalized = نادرست;
		بازگشت;
	}

	اگر (!flagBufferedNormalized)
	{
		برای (auto module : modules())
			buf_norm_seed_queues(module);

		flagBufferedNormalized = درست;
	}

	برای (auto module : modules())
		module->bufNormalize();
}

ساختار bit_drive_data_t {
	صحیح drivers = 0;
	صحیح inout = 0;
	صحیح users = 0;
};

تعریفنوع ModWalker::PortBit PortBit;

تهی RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#define xlog(...) انجامبده { اگر (yosys_xtrace) log("#X [bufnorm] " __VA_ARGS__); } وقتی (0)

	اگر (!design->flagBufferedNormalized)
		بازگشت;

	اگر (!buf_norm_cell_queue.empty() || !buf_norm_wire_queue.empty() || !connections_.empty())
	{
		// Ensure that every enqueued input port is represented by a cell
		برای (auto wire : buf_norm_wire_queue) {
			اگر (wire->port_درون_داد && !wire->port_output && GetSize(wire) != 0) {
				اگر (wire->driverCell_ != nullptr && wire->driverCell_->type != ID($درون_داد_port)) {
					wire->driverCell_ = nullptr;
					wire->driverPort_ = IdString::Null;
				}
				اگر (wire->driverCell_ == nullptr) {
					Cell *درون_داد_port_cell = addCell(NEW_ID, ID($درون_داد_port));
					درون_داد_port_cell->setParam(ID::WIDTH, GetSize(wire));
					درون_داد_port_cell->setPort(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
				}
			}
		}

		// Next we will temporarily undo buf normalization locally for
		// everything enqueued. This means we will turn $buf and $connect back
		// into connections. When doing this we also need to enqueue the other
		// end of $buf and $connect cells, so we use a queue and do this until
		// reaching a fixed point.

		// While doing this, we will also discover all drivers fully connected
		// to enqueued wires. We keep track of which wires are driven by a
		// unique and full cell ports (in which case the wire can stay
		// connected to the port) and which cell ports will need to be
		// reconnected to a fresh intermediate wire to re-normalize the module.

		idict<Wire *> wire_queue_entries; // Ordered queue of wires to process
		صحیح wire_queue_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> direct_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		auto enqueue_cell_port = [&](Cell *cell, IdString port) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(port));

			// An empty cell type means the cell got removed
			اگر (cell->type.empty())
				بازگشت;


			SigSpec ثابت &sig = cell->getPort(port);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			برای (auto chunk : sig.chunks())
				اگر (chunk.is_wire())
					wire_queue_entries(chunk.wire);

			اگر (cell->type == ID($buf) && cell->attributes.empty() && !cell->name.isPublic()) {
				// For a plain `$buf` cell, we enqueue all wires on its input
				// side, bypass it using module level connections (skipping 'z
				// bits) and then remove the cell. Eventually the module level
				// connections will turn back into `$buf` and `$connect` cells,
				// but since we also need to handle externally added module
				// level connections, turning everything into connections first
				// simplifies the logic for doing so.

				// TODO: We could defer removing the $buf cells here, and
				// re-use them in case we would create a new identical cell
				// later.
				log_assert(port == ID::Y);
				SigSpec sig_a = cell->getPort(ID::A);
				SigSpec sig_y = sig;

				برای (auto ثابت &s : {sig_a, sig})
					برای (auto ثابت &chunk : s.chunks())
						اگر (chunk.wire)
							wire_queue_entries(chunk.wire);

				اگر (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					برای (صحیح i = 0; i < GetSize(sig_a); ++i) {
						SigBit b = sig_a[i];
						اگر (b == State::Sz)
							ادامه;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				اگر (!sig_y.empty())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(GetSize(buf_norm_wire_queue) <= 1);
				buf_norm_wire_queue.clear();
				بازگشت;
			} وگرنه اگر (cell->type == ID($درون_داد_port)) {
				log_assert(port == ID::Y);
				اگر (sig.is_wire()) {
					Wire *w = sig.as_wire();
					اگر (w->port_درون_داد && !w->port_output) {
						// An $input_port cell can only drive a full wire module input port
						auto [found, inserted] = direct_driven_wires.emplace(w, {cell, port});
						اگر (!inserted || (found->second.first == cell && found->second.second == port))
							بازگشت;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_queue.درج(cell);
				remove(cell);
				log_assert(GetSize(buf_norm_wire_queue) <= 1);
				buf_norm_wire_queue.clear();
				بازگشت;
			}

			اگر (sig.is_wire()) {
				Wire *w = sig.as_wire();
				اگر (!w->port_درون_داد || w->port_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					auto [found, inserted] = direct_driven_wires.emplace(w, {cell, port});
					اگر (inserted || (found->second.first == cell && found->second.second == port))
						بازگشت;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, port);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		برای (auto wire : buf_norm_wire_queue)
			wire_queue_entries(wire);
		buf_norm_wire_queue.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		برای (auto ثابت &[cell, port_name] : buf_norm_cell_port_queue)
			enqueue_cell_port(cell, port_name);
		buf_norm_cell_port_queue.clear();

		// We also enqueue all wires that saw newly added module level connections.
		برای (auto &[a, b] : connections_)
			برای (auto &sig : {a, b})
				برای (auto ثابت &chunk : sig.chunks())
					اگر (chunk.wire)
						wire_queue_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		وقتی (wire_queue_pos < GetSize(wire_queue_entries)) {
			auto wire = wire_queue_entries[wire_queue_pos++];
			xlog("processing wire %s\n", wire);

			اگر (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString port = wire->driverPort_;
				enqueue_cell_port(cell, port);
			}

			وقتی (درست) {
				auto found = buf_norm_connect_index.find(wire);
				اگر (found == buf_norm_connect_index.end())
					بشکن;
				وقتی (!found->second.empty()) {
					Cell *connect_cell = *found->second.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ثابت &sig_a = connect_cell->getPort(ID::A);
					SigSpec ثابت &sig_b = connect_cell->getPort(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					برای (auto &side : {sig_a, sig_b})
						برای (auto chunk : side.chunks())
							اگر (chunk.wire)
								wire_queue_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_queue.درج(connect_cell);
					remove(connect_cell);
					log_assert(GetSize(buf_norm_wire_queue) <= 2);
					buf_norm_wire_queue.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		برای (auto &[cell, port] : pending_ports) {
			log_assert(cell->type != ID($درون_داد_port));
			log_assert(!cell->type.empty());
			log_assert(!pending_deleted_cells.شمار(cell));
			SigSpec ثابت &sig = cell->getPort(port);
			Wire *w = addWire(NEW_ID, GetSize(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			direct_driven_wires.emplace(w, {cell, port});
			cell->setPort(port, w); // Hits the fast path that doesn't enqueue w
			wire_queue_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		برای (auto wire : wire_queue_entries) {
			wire->driverCell_ = nullptr;
			wire->driverPort_ = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		برای (auto ثابت &[wire, cellport] : direct_driven_wires) {
			wire->driverCell_ = cellport.first;
			wire->driverPort_ = cellport.second;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		اگر (yosys_xtrace)
			برای (auto ثابت &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(این);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		برای (منطقی درون_داد_ports : {نادرست, درست}) {
			برای (auto ثابت &[wire, cellport] : direct_driven_wires) {
				اگر ((wire->port_درون_داد && !wire->port_output) == درون_داد_ports) {
					برای (صحیح i = 0; i != GetSize(wire); ++i) {
						SigBit driver = SigBit(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<SigBit> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<SigBit> conflicted;
		// Bits that are driven by an inout driver
		pool<SigBit> weakly_driven;

		برای (auto ثابت &[wire, cellport] : direct_driven_wires) {
			auto ثابت &[cell, port] = cellport;
			برای (صحیح i = 0; i != GetSize(wire); ++i) {
				SigBit driver = sigmap(SigBit(wire, i));
				اگر (cell->type == ID($tribuf) || cell->port_dir(port) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.درج(driver);
				} وگرنه {
					// We remove driver conflicts from `driven` in a separate loop below
					منطقی inserted = driven.درج(driver).second;
					اگر (!inserted)
						conflicted.درج(driver);
				}
			}
		}

		برای (auto driver : weakly_driven)
			driven.درج(driver);

		// This only leaves the drivers matching `driven`'s definition above
		برای (auto driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<SigBit, SigBit>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		برای (auto wire : wire_queue_entries) {
			// A zero-width wire carries no bits and needs no driver
			اگر (GetSize(wire) == 0)
				ادامه;

			SigSpec wire_drivers;
			برای (صحیح i = 0; i < GetSize(wire); ++i) {
				SigBit bit(wire, i);
				SigBit mapped = sigmap(bit);
				xlog("bit %s -> mapped %s\n", log_signal(bit), log_signal(mapped));


				buf_connected.apply(bit);
				buf_connected.add(bit, mapped);
				buf_connected.database.promote(mapped);

				اگر (wire->driverCell_ == nullptr) {
					اگر (!mapped.is_wire() || driven.شمار(mapped)) {
						wire_drivers.append(mapped);
						ادامه;
					} وگرنه {
						wire_drivers.append(State::Sz);
					}
				}

				اگر (bit < mapped)
					undirected_connections.emplace(bit, mapped);
				وگرنه اگر (mapped < bit)
					undirected_connections.emplace(mapped, bit);
			}

			اگر (wire->driverCell_ == nullptr) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				addBuf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		ایستا auto sort_کلید = [](std::pair<SigBit, SigBit> ثابت &p) {
			صحیح first_offset = p.first.is_wire() ? p.first.offset : 0;
			صحیح second_offset = p.second.is_wire() ? p.second.offset : 0;
			بازگشت std::make_tuple(p.first.wire, p.second.wire, first_offset - second_offset, p);
		};

		undirected_connections.sort([](std::pair<SigBit, SigBit> ثابت &p, std::pair<SigBit, SigBit> ثابت &q) {
			بازگشت sort_کلید(p) < sort_کلید(q);
		});

		SigSpec tmp_a, tmp_b;

		برای (auto &[bit_a, bit_b] : undirected_connections) {
			tmp_a.append(bit_a);
			tmp_b.append(bit_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		SigBit next_a, next_b;

		auto emit_connect_cell = [&]() {
			اگر (sig_a.empty())
				بازگشت;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = addCell(NEW_ID, ID($connect));
			connect_cell->setParam(ID::WIDTH, GetSize(sig_a));
			connect_cell->setPort(ID::A, sig_a);
			connect_cell->setPort(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		برای (auto &[bit_a, bit_b] : undirected_connections) {
			اگر (bit_a == bit_b)
				ادامه;
			اگر (bit_a != next_a || bit_b != next_b)
				emit_connect_cell();

			sig_a.append(bit_a);
			sig_b.append(bit_b);
			next_a = bit_a;
			next_b = bit_b;
			اگر (next_a.is_wire())
				next_a.offset++;
			اگر (next_b.is_wire())
				next_b.offset++;

		}
		emit_connect_cell();

		buf_norm_cell_queue.clear();

		log_assert(buf_norm_cell_port_queue.empty());
		log_assert(buf_norm_wire_queue.empty());
		log_assert(connections_.empty());
	}

	برای (auto cell : pending_deleted_cells) {
		delete cell;
	}
	pending_deleted_cells.clear();
}

تهی RTLIL::Cell::unsetPort(RTLIL::IdString portname)
{
	RTLIL::SigSpec سیگنال;
	auto conn_it = connections_.find(portname);

	اگر (conn_it != connections_.end())
	{
		برای (auto mon : module->monitors)
			mon->notify_connect(این, conn_it->first, conn_it->second, سیگنال);

		اگر (module->design)
			برای (auto mon : module->design->monitors)
				mon->notify_connect(این, conn_it->first, conn_it->second, سیگنال);

		اگر (yosys_xtrace) {
			log("#X# Unconnect %s.%s.%s\n", این->module, این, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		اگر (module->design && module->design->flagBufferedNormalized) {
			اگر (conn_it->second.is_wire()) {
				Wire *w = conn_it->second.as_wire();
				اگر (w->driverCell_ == این && w->driverPort_ == portname) {
					w->driverCell_ = nullptr;
					w->driverPort_ = IdString();
					module->buf_norm_wire_queue.درج(w);
				} وگرنه اگر (w->driverCell_) {
					log_assert(w->driverCell_->getPort(w->driverPort_) == w);
				}
			}

			اگر (type == ID($connect)) {
				برای (auto &[port, sig] : connections_) {
					برای (auto &chunk : sig.chunks()) {
						اگر (!chunk.wire)
							ادامه;
						auto it = module->buf_norm_connect_index.find(chunk.wire);
						اگر (it == module->buf_norm_connect_index.end())
							ادامه;
						it->second.erase(این);
						اگر (it->second.empty())
							module->buf_norm_connect_index.erase(it);
					}
				}
				connections_.erase(conn_it);
				برای (auto &[port, sig] : connections_) {
					برای (auto &chunk : sig.chunks()) {
						اگر (!chunk.wire)
							ادامه;
						module->buf_norm_connect_index[chunk.wire].درج(این);
					}
				}
				بازگشت;
			}
		}

		connections_.erase(conn_it);
	}
}

تهی RTLIL::Cell::setPort(RTLIL::IdString portname, RTLIL::SigSpec سیگنال)
{
	auto r = connections_.درج(portname);
	auto conn_it = r.first;
	اگر (!r.second && conn_it->second == سیگنال)
		بازگشت;

	برای (auto mon : module->monitors)
		mon->notify_connect(این, conn_it->first, conn_it->second, سیگنال);

	اگر (module->design)
		برای (auto mon : module->design->monitors)
			mon->notify_connect(این, conn_it->first, conn_it->second, سیگنال);

	اگر (yosys_xtrace) {
		log("#X# Connect %s.%s.%s = %s (%d)\n", این->module, این, PooledName(module, portname).unescape(), log_signal(سیگنال), GetSize(سیگنال));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	اگر (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		اگر (conn_it->second.is_wire()) {
			Wire *w = conn_it->second.as_wire();
			اگر (w->driverCell_ == این && w->driverPort_ == portname) {
				w->driverCell_ = nullptr;
				w->driverPort_ = IdString();
				module->buf_norm_wire_queue.درج(w);
			}
		}

		auto dir = port_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		اگر ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && سیگنال.is_wire()) {
			Wire *w = سیگنال.as_wire();
			اگر (w->driverCell_ == nullptr && (
						(w->port_درون_داد && !w->port_output) == (type == ID($درون_داد_port)))) {
				w->driverCell_ = این;
				w->driverPort_ = portname;

				conn_it->second = std::move(سیگنال);
				بازگشت;
			}
		}

		اگر (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_queue.درج(این);
			module->buf_norm_cell_port_queue.emplace(این, portname);
		} وگرنه {
			برای (auto &chunk : سیگنال.chunks())
				اگر (chunk.wire != nullptr && chunk.wire->driverCell_ == nullptr)
					module->buf_norm_wire_queue.درج(chunk.wire);
		}

		اگر (type == ID($connect)) {
			برای (auto &[port, sig] : connections_) {
				برای (auto &chunk : sig.chunks()) {
					اگر (!chunk.wire)
						ادامه;
					auto it = module->buf_norm_connect_index.find(chunk.wire);
					اگر (it == module->buf_norm_connect_index.end())
						ادامه;
					it->second.erase(این);
					اگر (it->second.empty())
						module->buf_norm_connect_index.erase(it);
				}
			}
			conn_it->second = std::move(سیگنال);
			برای (auto &[port, sig] : connections_) {
				برای (auto &chunk : sig.chunks()) {
					اگر (!chunk.wire)
						ادامه;
					module->buf_norm_connect_index[chunk.wire].درج(این);
				}
			}
			بازگشت;
		}
	}
	conn_it->second = std::move(سیگنال);

}

تهی RTLIL::Design::add(RTLIL::Module *module)
{
	log_assert(modules_.شمار(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = این;

	برای (auto mon : monitors)
		mon->notify_module_add(module);

	اگر (yosys_xtrace) {
		log("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	اگر (flagBufferedNormalized) {
		buf_norm_seed_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_END
