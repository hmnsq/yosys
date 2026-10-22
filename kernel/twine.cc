#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template بنية HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	𐤀𐤌 (ready())
		أرجع;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	لكل (ثابت حرف *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ثابت TwineNode &StaticTwines::node(size_t idx) { أرجع nodes_[idx]; }
منطقي StaticTwines::ready() { أرجع nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
صحيح twine_gc_count;

std::string IdString::handle_token() ثابت {
	أرجع stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, منطقي &is_public) {
	𐤀𐤌 (token.substr(0, 5) == "$pub@") {
		is_public = true;
		أرجع 5;
	}
	𐤀𐤌 (token.substr(0, 6) == "$priv@") {
		is_public = خطأ;
		أرجع 6;
	}
	أرجع 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	أرجع result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	أرجع static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	𐤀𐤌 (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		memcpy(ptr_, content.data(), len_);
	} وإلا {
		memcpy(inl_, content.data(), len_);
	}
}

فراغ SmallString::release()
{
	𐤀𐤌 (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ثابت SmallString &other)
{
	𐤀𐤌 (هذا != &other)
		store(other.view());
	أرجع *هذا;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	𐤀𐤌 (هذا == &other)
		أرجع *هذا;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	أرجع *هذا;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	𐤀𐤌 (هذا == &other)
		أرجع *هذا;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	أرجع *هذا;
}

std::string TwineSpec::content_str() ثابت {
	𐤀𐤌 (auto *leaf = std::get_if<Leaf>(&data))
		أرجع leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	أرجع *autosfx.prefix + autosfx.tail;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	𐤀𐤌 (!name.empty() && name[0] == '\\')
		أرجع {name.substr(1), true};
	أرجع {name, خطأ};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	لكل (حرف ch : name)
		𐤀𐤌 ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ثابت TwinePool& other) {
	𐤀𐤌 (هذا == &other)
		أرجع *هذا;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	أرجع *هذا;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	𐤀𐤌 (هذا == &other)
		أرجع *هذا;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	أرجع *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	أرجع ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	𐤀𐤌 (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	أرجع ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	𐤀𐤌 constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت TwineNode& TwinePool::operator[](IdString ref) ثابت {
	check_owned(ref);
	أرجع HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	لكل (auto &it : auto_prefixes)
		roots.insert(it.second);
	أرجع HashConsPool::gc(roots);
}

ثابت TwineNode& TwinePool::static_node(size_t idx) { أرجع StaticTwines::node(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	ساكن size_t counter = 0;
	size_t serial = ++counter;
	𐤀𐤌 constexpr (IdString::MAX_SERIAL == 0)
		أرجع serial;
	وإلا
		أرجع serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	𐤀𐤌 (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	أرجع HashConsPool::add_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت TwineNode& twine = (*هذا)[ref];
	بدل (twine.kind()) {
	حالة TwineNode::Kind::Dead:
		os << "Dead()";
		اكسر;
	حالة TwineNode::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		اكسر;
	حالة TwineNode::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		اكسر;
	}
	𐤀𐤌 (ref.isPublic())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& out) ثابت {
	𐤀𐤌 (ref == IdString::Null)
		أرجع;
	𐤀𐤌 (ref.isPublic())
		out += '\\';
	ثابت TwineNode& twine = (*هذا)[ref];
	بدل (twine.kind()) {
	حالة TwineNode::Kind::Dead:
		اكسر;
	حالة TwineNode::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	حالة TwineNode::Kind::Leaf:
		out += twine.text();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string out;
	append_str(ref, out);
	أرجع out;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	أرجع str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ثابت {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*هذا)[IdString(prefix)].hash();
	أرجع TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ثابت {
	أرجع find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	𐤀𐤌 (ref != IdString::Null)
		أرجع ref;
	أرجع add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ثابت {
	𐤀𐤌 (name.empty())
		أرجع IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	أرجع stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ثابت {
	𐤀𐤌 (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		أرجع stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	𐤀𐤌 (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		أرجع stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	أرجع stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	𐤀𐤌 (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		أرجع add(auto_prefix(ap->prefix), ap->tail);
	𐤀𐤌 (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		أرجع stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	أرجع add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ثابت std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	𐤀𐤌 (it != auto_prefixes.end())
		أرجع it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	أرجع ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	أرجع stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	𐤀𐤌 (s.empty())
		أرجع IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	أرجع stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	𐤀𐤌 (found != IdString::Null || ref == IdString::Null)
		أرجع found;
	ثابت TwineNode& t = src[ref.untag()];
	𐤀𐤌 (t.is_dead())
		أرجع IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	𐤀𐤌 (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	أرجع stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	أرجع "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	𐤀𐤌 (token.size() < 2 || token[0] != '#')
		أرجع IdString::Null;
	size_t sep = token.find(':');
	𐤀𐤌 (sep == std::string_view::npos)
		أرجع IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	لكل (صحيح i = 0; i < 2; i++) {
		𐤀𐤌 (parts[i].empty())
			أرجع IdString::Null;
		لكل (حرف c : parts[i]) {
			𐤀𐤌 (c < '0' || c > '9')
				أرجع IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	𐤀𐤌 (fields[0] != serial_)
		أرجع IdString::Null;
	IdString ref(fields[1]);
	𐤀𐤌 (ref == IdString::Null)
		أرجع IdString::Null;
	أرجع is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	فراغ advance(size_t n);
	size_t total_size() ثابت;
خاص:
	فراغ push(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t count = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	𐤀𐤌 (ref == IdString::Null)
		أرجع;
	لكل (IdString cur = ref.untag(); ;) {
		ثابت TwineNode &t = pool[cur];
		بدل (t.kind()) {
		حالة TwineNode::Kind::Suffix:
			push(t.text());
			cur = t.prefix();
			استمر;
		حالة TwineNode::Kind::Leaf:
			push(t.text());
			اكسر;
		حالة TwineNode::Kind::Dead:
			اكسر;
		}
		اكسر;
	}
	𐤀𐤌 (ref.isPublic())
		push("\\");
	std::reverse(segs(), segs() + count);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	push(tail);
}

std::string_view TwineSegments::peek()
{
	طالما (pos < count) {
		std::string_view seg = segs()[pos];
		𐤀𐤌 (off < seg.size())
			أرجع seg.substr(off);
		pos++;
		off = 0;
	}
	أرجع {};
}

فراغ TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ثابت
{
	size_t total = 0;
	لكل (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	أرجع total;
}

فراغ TwineSegments::push(std::string_view seg)
{
	𐤀𐤌 (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		أرجع;
	}
	𐤀𐤌 (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { أرجع spill.empty() ? inline_segs : spill.data(); }
ثابت std::string_view *TwineSegments::segs() ثابت { أرجع spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ثابت
{
	أرجع TwineSegments(*هذا, ref).total_size();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view prefix) ثابت
{
	TwineSegments segs(*هذا, ref);
	طالما (!prefix.empty()) {
		std::string_view seg = segs.peek();
		𐤀𐤌 (seg.empty())
			أرجع خطأ;
		size_t n = std::min(seg.size(), prefix.size());
		𐤀𐤌 (std::memcmp(seg.data(), prefix.data(), n) != 0)
			أرجع خطأ;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	أرجع true;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	أرجع str_size(ref) == name.size() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	طالما (true) {
		std::string_view x = sa.peek(), y = sb.peek();
		𐤀𐤌 (x.empty() || y.empty())
			أرجع x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		𐤀𐤌 (صحيح diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			أرجع diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ثابت {
	أرجع find_hashed(hash, [&](IdString ref) {
		ثابت TwineNode &t = (*هذا)[ref];
		𐤀𐤌 (t == TwineNode::Key{prefix, text})
			أرجع true;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*هذا, ref), sb(*هذا, head, text);
		أرجع compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ثابت TwinePool& src, IdString ref) ثابت {
	𐤀𐤌 (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	IdString idx = ref.untag();
	ثابت TwineNode& t = src[idx];
	𐤀𐤌 (t.is_dead())
		أرجع IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		أرجع compare_segments(sa, sb) == 0;
	});
	أرجع stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ثابت {
	ثابت TwineNode &t = (*هذا)[ref];
	𐤀𐤌 (!t.is_suffix())
		أرجع IdString::Null;
	أرجع stamp(t.prefix().tag(ref.isPublic()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	𐤀𐤌 (a == b)
		أرجع 0;
	𐤀𐤌 (a == IdString::Null)
		أرجع -1;
	𐤀𐤌 (b == IdString::Null)
		أرجع 1;

	𐤀𐤌 (a.isPublic() == b.isPublic()) {
		ثابت TwineNode &ta = (*هذا)[a.untag()];
		ثابت TwineNode &tb = (*هذا)[b.untag()];
		𐤀𐤌 (ta.is_leaf() && tb.is_leaf())
			أرجع ta.text().compare(tb.text());
		𐤀𐤌 (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			أرجع ta.text().compare(tb.text());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	أرجع compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
