package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;

/**
 * Base class representing a CLP-encoded subquery. Each subquery encompasses a
 * single logtype query and zero or more variable queries. Both the logytpe
 * and variables may contain these wildcards:
 * <ul>
 *   <li>'?' matching any single character</li>
 *   <li>'*' matching 0 or more characters</li>
 * </ul>
 */
public abstract class AbstractClpEncodedSubquery {
  // NOTE: This must be kept in-sync with CLP's built-in variable schemas.
  private static final byte VAR_SCHEMAS_V2_DICT_VAR_PLACEHOLDER = 0x12;

  // For a description of these member variables, see the constructor.
  private final byte[] query;
  private final byte[] logtypeQuery;
  private final boolean logtypeQueryContainsWildcards;
  private final int[] dictVarBounds;
  private final byte[] wildcardVarPlaceholders;
  //
  private final int[] wildcardVarBounds;

  /**
   * @param query The parent wildcard query for this subquery
   * @param logtypeQuery The logtype query from encoding the parent query
   * @param logtypeQueryContainsWildcards Whether logtypeQuery contains
   * wildcards
   * @param dictVarBounds The "begin" and "end" index of each dictionary
   * variable, laid out as follows: [v1.begin, v1.end, v2.begin, v2.end, ...]
   * This layout is more efficient to transfer across the JNI boundary.
   * @param wildcardVarPlaceholders An array of placeholders, one for each
   * variable (or potential variable) containing a wildcard, indicating how the
   * variable query should be interpreted.
   * @param wildcardVarBounds The "begin" and "end" index of each
   * wildcard-containing variable, laid out as follows:
   * [v1.begin, v1.end, v2.begin, v2.end, ...]. This layout is more efficient to
   * transfer across the JNI boundary.
   */
  public AbstractClpEncodedSubquery (byte[] query, byte[] logtypeQuery,
                                     boolean logtypeQueryContainsWildcards, int[] dictVarBounds,
                                     byte[] wildcardVarPlaceholders, int[] wildcardVarBounds)
  {
    this.query = query;
    this.logtypeQuery = logtypeQuery;
    this.logtypeQueryContainsWildcards = logtypeQueryContainsWildcards;
    this.dictVarBounds = dictVarBounds;
    this.wildcardVarPlaceholders = wildcardVarPlaceholders;
    this.wildcardVarBounds = wildcardVarBounds;
  }

  public String getLogtypeQueryAsString () {
    return new String(logtypeQuery, StandardCharsets.ISO_8859_1);
  }

  public boolean logtypeQueryContainsWildcards () {
    return logtypeQueryContainsWildcards;
  }

  /**
   * @return An iterable of the dictionary variables as byte segments
   */
  public ByteSegments getDictVars () {
    return new ByteSegments(query, dictVarBounds);
  }

  public int getNumDictVars () {
    return dictVarBounds.length / 2;
  }

  /**
   * @return An iterable of the wildcard-containing variables that should be
   * interpreted as dictionary variables
   */
  public DictionaryVariableWildcardQueries getDictVarWildcardQueries () {
    return new DictionaryVariableWildcardQueries();
  }

  public int getNumDictVarWildcardQueries () {
    int count = 0;
    for (VariableWildcardQuery ignored : getDictVarWildcardQueries()) {
      ++count;
    }
    return count;
  }

  /**
   * @return An iterable of the wildcard-containing variables that should be
   * interpreted as encoded variables
   */
  public EncodedVariableWildcardQueries getEncodedVarWildcardQueries () {
    return new EncodedVariableWildcardQueries();
  }

  public int getNumEncodedVarWildcardQueries () {
    int count = 0;
    for (VariableWildcardQuery ignored : getEncodedVarWildcardQueries()) {
      ++count;
    }
    return count;
  }

  protected boolean containsVariables () {
    return dictVarBounds.length > 0 || wildcardVarPlaceholders.length > 0;
  }

  /**
   * Iterable class for the wildcard-containing variables that should be
   * interpreted as dictionary variables
   */
  public class DictionaryVariableWildcardQueries extends VariableWildcardQueries {
    @Override
    public boolean isRelevantType (byte type) {
      return VAR_SCHEMAS_V2_DICT_VAR_PLACEHOLDER == type;
    }
  }

  /**
   * Iterable class for the wildcard-containing variables that should be
   * interpreted as encoded variables
   */
  public class EncodedVariableWildcardQueries extends VariableWildcardQueries {
    @Override
    public boolean isRelevantType (byte type) {
      return VAR_SCHEMAS_V2_DICT_VAR_PLACEHOLDER != type;
    }
  }

  /**
   * Base iterable class for the wildcard-containing variables
   */
  public abstract class VariableWildcardQueries implements Iterable<VariableWildcardQuery> {
    @Override
    public Iterator iterator () {
      return new Iterator();
    }

    public class Iterator implements java.util.Iterator<VariableWildcardQuery> {
      private int substringBoundsIdx = 0;
      private int varIdx = 0;

      @Override
      public boolean hasNext () {
        while (varIdx < wildcardVarPlaceholders.length
            && false == isRelevantType(wildcardVarPlaceholders[varIdx]))
        {
          ++varIdx;
          substringBoundsIdx += 2;
        }
        return (wildcardVarPlaceholders.length != varIdx);
      }

      @Override
      public VariableWildcardQuery next () {
        int beginIdx = wildcardVarBounds[substringBoundsIdx++];
        int endIdx = wildcardVarBounds[substringBoundsIdx++];
        return new VariableWildcardQuery(wildcardVarPlaceholders[varIdx++],
                                         new ByteSegment(query, beginIdx, endIdx));
      }
    }

    /**
     * @param type Type of the variable wildcard query
     * @return Whether the type corresponds to variable queries relevant
     * to this iterable
     */
    public abstract boolean isRelevantType (byte type);
  }

  /**
   * A wildcard query for a variable with a certain type. Callers should not
   * rely on the particular value of the type since it may change as the
   * built-in schemas change.
   */
  public class VariableWildcardQuery {
    private final byte type;
    private final ByteSegment query;

    public VariableWildcardQuery (byte type, ByteSegment query) {
      this.type = type;
      this.query = query;
    }

    public byte getType () {
      return type;
    }

    public ByteSegment getQuery () {
      return query;
    }
  }
}
